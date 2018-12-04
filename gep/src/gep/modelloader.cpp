#include "stdafx.h"
#include "gep/exception.h"
#include "gep/utils.h"
#include "gep/modelloader.h"

#include "thModelloader.inl"
#include "AssimpModelloader.inl"

// Common implementation
//////////////////////////////////////////////////////////////////////////

gep::ModelLoader::ModelLoader(IAllocator* pAllocator) :
    m_pModelDataAllocator(nullptr),
    m_pStartMarker(nullptr)
{
    if(pAllocator == nullptr)
        pAllocator = &g_stdAllocator;
    m_pAllocator = pAllocator;
}

gep::ModelLoader::~ModelLoader()
{
    if(m_pModelDataAllocator != nullptr)
    {
        m_pModelDataAllocator->freeToMarker(m_pStartMarker);
        GEP_DELETE(m_pAllocator, m_pModelDataAllocator);
    }
}

void gep::ModelLoader::loadFile(const char* pFilename, uint32 loadWhat)
{
    GEP_ASSERT(!m_modelData.hasData,"LoadFile can only be called once");
    m_filename = pFilename;

    // Check if the file actually exists.
    if(!fileExists(pFilename))
    {
        std::ostringstream msg;
        msg << "File '" << pFilename << "' does not exist";
        throw LoadingError(msg.str());
    }

    auto szFileExtension = gep::findLast(
        pFilename,                     // `begin`
        pFilename + strlen(pFilename), // `end`
        '.');                          // What to look for.

    if (strcmp(szFileExtension, ".thModel") == 0)
    {
        // If the file extension is .thModel, we try to load a thModel...
        loadThModel(pFilename, loadWhat);
    }
    else
    {
        unsigned int nameHash = gep::hashOf( pFilename, strlen(pFilename) );
        std::string hashPath = std::string( ".modelCache/" ) + std::to_string( nameHash ) + std::string( ".thModel" );
        std::string hashInfoPath = hashPath + std::string( ".info" );

        gep::RawFile file;
        file.open( pFilename, "rb" );
        uint32 size = file.getSize();
        ArrayPtr<uint8> assimpFileData= GEP_NEW_ARRAY( m_pAllocator, uint8, size );

        file.readArray( assimpFileData.getPtr(), assimpFileData.length() );

        file.close();

        unsigned int hash = gep::hashOf( assimpFileData.getPtr(), assimpFileData.length() );

        GEP_DELETE_ARRAY( m_pAllocator, assimpFileData );

        bool loadCache = false;

        if( gep::fileExists( hashPath.c_str()) )
        {
            if( !gep::fileExists( hashInfoPath.c_str() ) )
            {
                g_globalManager.getLogging()->logWarning(
                    "Found cached file of %s, but no information file, something seems wrong, recreating cache....", pFilename );
            }
            else
            {
                gep::RawFile infoFile;
                infoFile.open( hashInfoPath.c_str(), "rb" );
                HashInfoFileData data;
                memset( &data, 0, sizeof( HashInfoFileData ) );

                infoFile.read( data );

                infoFile.close();
                bool conflict = strcmp( pFilename, data.path ) != 0;
                loadCache = data.hash == hash && !conflict;


                GEP_ASSERT( !conflict,
                    "==============================================\n"
                    "== Hash conflict!!! \n"
                    "== File: %s has the same hash as %s!!! \n"
                    "== You need to rename on of those files! \n"
                    "==============================================\n", pFilename, data.path );

                if( data.hash != hash )
                {
                    g_globalManager.getLogging()->logMessage( "Change of source file '%s' detected, recreating cache...", pFilename );
                }

            }
        }

        if( loadCache )
        {
            g_globalManager.getLogging()->logMessage( "Loading cached file for %s (%s)", pFilename, hashPath.c_str() );
            loadThModel( hashPath.c_str(), loadWhat );
        }
        else
        {

            // ... else, we try to load the file using assimp.
            loadAssimpCompatibleModel(pFilename, loadWhat);

            createDirectory( ".modelCache" );

            writeThModel( hashPath.c_str() );

            gep::RawFile infoFile;
            infoFile.open( hashInfoPath.c_str(), "wb" );
            HashInfoFileData infoData;
            memset( &infoData, 0, sizeof( HashInfoFileData ) );
            infoData.hash = hash;
            strcpy_s( infoData.path, pFilename );

            infoFile.write( infoData );

            infoFile.close();

        }
    }
}


void gep::ModelLoader::writeThModel( const char* fileName )
{
    Chunkfile file( fileName, Chunkfile::Operation::write );

    file.startWriting( "thModel", ModelFormatVersion::Version3 );
    PerVertexData::Enum texCoords [] = {
        PerVertexData::TexCoord0,
        PerVertexData::TexCoord1,
        PerVertexData::TexCoord2,
        PerVertexData::TexCoord3
    };

    std::function<void( NodeDrawData*, std::function<void( NodeDrawData* )> )> traverseNodes = [ &]( NodeDrawData* myNode, std::function<void( NodeDrawData* )> nodeAction )
    {
        nodeAction( myNode );
        for( auto* child : myNode->children )
        {
            GEP_ASSERT( child != nullptr, "Child was nullptr but parent said there would be a child!" );
            traverseNodes( child, nodeAction );
        }
    };

    {
        file.startWriteChunk( "sizeinfo" );
        uint32 texturePathMemory = 0;
        for( const char* path : m_modelData.textures )
        {
            texturePathMemory += strlen( path ) + 1;
        }
        file.write( ( uint32 ) m_modelData.textures.length() );
        file.write( ( uint32 ) texturePathMemory );

        uint32 materialNameMemory = 0;
        uint32 numTextureReferences = 0;
        for( MaterialData& material : m_modelData.materials )
        {
            materialNameMemory += strlen( material.name ) + 1;
            numTextureReferences += material.textures.length();
        }
        file.write( ( uint32 ) materialNameMemory );

        // Num Bones
        file.write( ( uint32 ) m_modelData.bones.length() );
        // Num Bone infos
        uint32 numBoneInfos = 0;
        for( auto& data : m_modelData.meshes )
        {
            numBoneInfos += data.boneInfos.length();
        }
        file.write( ( uint32 ) numBoneInfos );

        file.write( ( uint32 ) m_modelData.materials.length() );

        file.write( ( uint32 ) m_modelData.meshes.length() );

        for( auto& mesh : m_modelData.meshes )
        {
            file.write( ( uint32 ) mesh.vertices.length() );
            file.write( ( uint32 ) mesh.PerVertexFlags );

            for( uint32 i = 0; i < 4; i++ )
            {
                if( ( mesh.PerVertexFlags & texCoords[ i ] ) != 0 )
                {
                    // We always have two components per texcoord
                    file.write( ( uint8 ) 2 );
                }
            }

            file.write( ( uint32 ) mesh.faces.length() );
        }

        uint32 numNodes = 0;
        uint32 numNodeReferences = 0;
        uint32 numMeshReferences = 0;
        uint32 nodeNameMemory = 0;

        std::function< void( NodeDrawData* ) > nodeSizeHelper = [ &]( NodeDrawData* node )
        {
            if( node == nullptr )
            {
                return;
            }
            numNodes++;
            numNodeReferences += node->children.length();
            numMeshReferences += node->meshes.length();
            nodeNameMemory += strlen( node->data->name ) + 1;
            for( auto* child : node->children )
            {
                nodeSizeHelper( child );
            }
        };

        nodeSizeHelper( m_modelData.rootNode );

        file.write( numNodes );
        file.write( numNodeReferences );
        file.write( nodeNameMemory );
        file.write( numMeshReferences );
        file.write( numTextureReferences );

        file.endWriteChunk();
    }
    {
        file.startWriteChunk( "textures" );

        file.write( ( uint32 ) m_modelData.textures.length() );
        for( auto* texture : m_modelData.textures )
        {
            file.writeArrayWithLength<const char, uint32>( ArrayPtr<const char>( texture, strlen( texture ) ) );
        }
        file.endWriteChunk();
    }
    {
        file.startWriteChunk( "materials" );

        file.write( ( uint32 ) m_modelData.materials.length() );
        for( auto& material : m_modelData.materials )
        {
            file.startWriteChunk( "mat" );
            file.writeArrayWithLength<const char, uint32>( ArrayPtr<const char>( material.name, strlen( material.name ) ) );
            file.write( ( uint32 ) material.textures.length() );
            for( TextureReference& reference : material.textures )
            {
                uint32 textureRefIndex = 0xFFFFFFFF;
                for( uint32 texRefIdx = 0; texRefIdx < m_modelData.textures.length(); texRefIdx++ )
                {
                    if( strcmp( reference.file, m_modelData.textures[ texRefIdx ] ) == 0 )
                    {
                        textureRefIndex = texRefIdx;
                        break;
                    }
                }
                GEP_ASSERT( textureRefIndex != 0xFFFFFFFF,
                    "A texture was referenced which was not capture before!" );
                file.write( ( uint32 ) textureRefIndex );
                file.write( ( uint8 ) reference.semantic );

            }
            file.endWriteChunk();
        }
        file.endWriteChunk();
    }

    auto compressFloat = []( float val )
    {
        return int16( val * std::numeric_limits<int16>::max() );
    };

    {

        uint32 idx = 0;
        uint32 foundIdx = -1;
        NodeDrawData* myData = nullptr;
        auto findIndex = [ &]( NodeDrawData* node )
        {
            if( node == myData )
            {
                foundIdx = idx;
            }
            idx++;
        };

        file.startWriteChunk("bones");

        file.write( ( uint32 ) m_modelData.bones.length() );
        for( uint32 boneIndex = 0; boneIndex < m_modelData.bones.length(); ++boneIndex )
        {
            auto& boneNode = m_modelData.bones[ boneIndex ];

            // Write offset matrix
            file.write( boneNode.offsetMatrix );

            idx = 0;
            foundIdx = -1;
            myData = boneNode.node;

            traverseNodes( m_modelData.rootNode, findIndex );

            GEP_ASSERT( foundIdx != -1, "Could not find index of node" );

            file.write( foundIdx );
        }

        file.endWriteChunk();

    }

    {
        file.startWriteChunk( "meshes" );
        file.write( ( uint32 ) m_modelData.meshes.length() );
        for( auto& mesh : m_modelData.meshes )
        {
            file.startWriteChunk( "mesh" );
            file.write( ( uint32 ) mesh.materialIndex );
            file.writeArray( makeArrayPtr( mesh.bbox.getMin().data ) );
            file.writeArray( makeArrayPtr( mesh.bbox.getMax().data ) );

            file.write( mesh.vertices.length() );

            file.startWriteChunk( "vertices" );
            static_assert( sizeof( float ) * 3 == sizeof( vec3 ), "The following line expects vectors to be sizeof 3 floats!" );
            file.writeArray( mesh.vertices );
            file.endWriteChunk();

            if( ( mesh.PerVertexFlags & PerVertexData::Normal ) != 0 )
            {
                file.startWriteChunk( "normals" );
                for( auto& normal : mesh.normals )
                {
                    file.write( compressFloat( normal.x ) );
                    file.write( compressFloat( normal.y ) );
                    file.write( compressFloat( normal.z ) );
                }
                file.endWriteChunk();
            }


            if( ( mesh.PerVertexFlags & PerVertexData::Tangent ) != 0 )
            {
                file.startWriteChunk( "tangents" );
                for( auto& tangent : mesh.tangents )
                {
                    file.write( compressFloat( tangent.x ) );
                    file.write( compressFloat( tangent.y ) );
                    file.write( compressFloat( tangent.z ) );
                }
                file.endWriteChunk();
            }

            if( ( mesh.PerVertexFlags & PerVertexData::Bitangent ) != 0 )
            {
                file.startWriteChunk( "bitangents" );
                for( auto& bitangent : mesh.bitangents )
                {
                    file.write( compressFloat( bitangent.x ) );
                    file.write( compressFloat( bitangent.y ) );
                    file.write( compressFloat( bitangent.z ) );
                }
                file.endWriteChunk();
            }

            {
                file.startWriteChunk( "texcoords" );

                uint32 numTexCoords = 0;

                for( uint32 i = 0; i < 4; i++ )
                {
                    if( ( mesh.PerVertexFlags & texCoords[ i ] ) != 0 )
                    {
                        numTexCoords++;
                    }
                }
                file.write( ( uint8 ) numTexCoords );
                for( uint32 texCoordIdx = 0; texCoordIdx < numTexCoords; texCoordIdx++ )
                {
                    auto& texCoords = mesh.texcoords[ texCoordIdx ];
                    file.write( ( uint8 ) 2 );
                    static_assert( sizeof( float ) * 2 == sizeof( vec2 ), "The following line expects vectors to be sizeof 2 floats!" );
                    file.writeArray( texCoords );
                }

                file.endWriteChunk();
            }

            {
                file.startWriteChunk( "bones" );

                file.write( ( uint32 ) mesh.boneInfos.length() );

                static_assert( sizeof( BoneInfo ) == sizeof( uint32 ) * 4 + sizeof( float ) * 4,
                    "The size of the internal BoneInfo struct has changed."
                    "You should check that the code here is still correct." );

                // Needed because the BoneInfo used by the engine is not the same size as the BoneInfo in the file.
                struct BoneInfoInFile
                {
                    uint16 boneIds[ BoneInfo::NUM_SUPPORTED_BONES ];
                    float weights[ BoneInfo::NUM_SUPPORTED_BONES ];
                };

                // Read all bone infos
                for( auto& boneInfo : mesh.boneInfos )
                {
                    BoneInfoInFile boneInfoIntermediate;
                    for( size_t vertexBoneIndex = 0; vertexBoneIndex < BoneInfo::NUM_SUPPORTED_BONES; ++vertexBoneIndex )
                    {
                        boneInfoIntermediate.boneIds[ vertexBoneIndex ] = boneInfo.boneIds[ vertexBoneIndex ];
                        boneInfoIntermediate.weights[ vertexBoneIndex ] = boneInfo.weights[ vertexBoneIndex ];
                    }
                    file.write( boneInfoIntermediate );
                }

                file.endWriteChunk();
            }

            {
                file.startWriteChunk( "faces" );

                file.write( ( uint32 ) mesh.faces.length() );
                if( mesh.vertices.length() > std::numeric_limits<uint16>::max() )
                {
                    static_assert( sizeof( uint32 ) * 3 == sizeof( FaceData ), "The following line expects FaceData to be sizeof 3 uint32!" );

                    file.writeArray( mesh.faces );
                }
                else
                {
                    for( auto& faceData : mesh.faces )
                    {
                        file.write( ( uint16 ) faceData.indices[ 0 ] );
                        file.write( ( uint16 ) faceData.indices[ 1 ] );
                        file.write( ( uint16 ) faceData.indices[ 2 ] );
                    }
                }

                file.endWriteChunk();
            }

            file.endWriteChunk();
        }
        file.endWriteChunk();
    }

    {
        file.startWriteChunk( "nodes" );

        uint32 nodeIdx = 0;
        uint32 numNodes = 0;

        gep::Hashmap< NodeDrawData*, int> nodeLookup;

        traverseNodes( m_modelData.rootNode, [ &]( NodeDrawData* node )
        {
            numNodes++;
            nodeLookup[ node ] = nodeIdx++;
        } );

        file.write( ( uint32 ) numNodes );

        traverseNodes( m_modelData.rootNode, [ &]( NodeDrawData* node )
        {
            file.writeArrayWithLength<const char, uint32>( makeArrayPtr( node->data->name, strlen( node->data->name ) + 1 ) );
            file.writeArray( makeArrayPtr( node->transform.data ) );
            if( node->parent == nullptr )
            {
                file.write( ( uint32 ) std::numeric_limits<uint32>::max() );
            }
            else
            {
                file.write( ( uint32 ) nodeLookup[ node->parent ] );
            }
            file.writeArrayWithLength<uint32, uint32>( node->meshes );
            file.write( ( uint32 ) node->children.length() );
            for( auto* child : node->children )
            {
                file.write( ( uint32 ) nodeLookup[ child ] );
            }

        } );

        file.endWriteChunk();
    }
    file.endWriting();
}
