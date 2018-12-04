
#include <gep/globalManager.h>

#include <gep/container/DynamicArray.h>
#include <gep/interfaces/logging.h>

#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "gep/timer.h"

namespace
{
    class AssimpLogger : public Assimp::Logger
    {
        static const char* const s_messagePrefix;

        struct RegisteredLogStream
        {
            Assimp::LogStream* pStream;
            unsigned int severity;

            RegisteredLogStream(Assimp::LogStream* pStream, unsigned int severity)
                : pStream(pStream)
                , severity(severity)
            {
            }

            inline bool isValid() const { return pStream != nullptr; }
            inline void setInvalid() { pStream = nullptr; }

            inline bool accepts(Assimp::Logger::ErrorSeverity severity) const
            {
                return (this->severity & severity) != 0;
            }

            inline bool operator==(const RegisteredLogStream& rhs)
            {
                return pStream == rhs.pStream
                       && severity == rhs.severity;
            }
        };

        gep::DynamicArray<RegisteredLogStream, gep::MallocAllocatorPolicy> m_logStreams;

    public:
        virtual bool attachStream(Assimp::LogStream* pStream, unsigned int severity = Debugging | Err | Warn | Info) override
        {
            GEP_ASSERT(pStream != nullptr, "Invalid stream.");
            RegisteredLogStream request(pStream, severity);
            for (auto& log : m_logStreams)
            {
                if (log == request)
                {
                    // Already attached.
                    return false;
                }
            }

            m_logStreams.append(request);
            return true;
        }

        virtual bool detatchStream(Assimp::LogStream* pStream, unsigned int severity = Debugging | Err | Warn | Info) override
        {
            GEP_ASSERT(pStream != nullptr, "Invalid stream.");
            RegisteredLogStream request(pStream, severity);
            for (auto& log : m_logStreams)
            {
                if (log == request)
                {
                    log.setInvalid();
                    return true;
                }
            }

            return false;
        }

        virtual void OnDebug(const char* message) override
        {
            g_globalManager.getLogging()->logMessage("%s Debug: %s", s_messagePrefix, message);
            Write(message, Debugging);
        }

        virtual void OnInfo(const char* message) override
        {
            g_globalManager.getLogging()->logMessage("%s %s", s_messagePrefix, message);
            Write(message, Info);
        }

        virtual void OnWarn(const char* message) override
        {
            g_globalManager.getLogging()->logWarning("%s %s", s_messagePrefix, message);
            Write(message, Warn);
        }

        virtual void OnError(const char* message) override
        {
            g_globalManager.getLogging()->logError("%s %s", s_messagePrefix, message);
            Write(message, Err);
        }

        void Write(const char* message, ErrorSeverity severity)
        {
            for (auto& log : m_logStreams)
            {
                if (log.isValid() && log.accepts(severity))
                {
                    log.pStream->write(message);
                }
            }
        }
    };
}

const char* const AssimpLogger::s_messagePrefix = "[Assimp]";

/// \note The lambda expression is immediately executed.
static AssimpLogger* g_pAssimpLogger = []
{
    static AssimpLogger l;
    l.setLogSeverity(Assimp::Logger::VERBOSE);
    Assimp::DefaultLogger::set(&l);
    return &l;
}();

inline void gep::ModelLoader::loadAssimpCompatibleModel(const char* pFilename, uint32 loadWhat)
{
    Timer timer;


    // Note: All resources allocated by the importer
    //       are destroyed when this scope is left.
    Assimp::Importer importer;

    importer.SetPropertyBool( AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, false );
    importer.SetPropertyBool( AI_CONFIG_IMPORT_FBX_OPTIMIZE_EMPTY_ANIMATION_CURVES, false );
    importer.SetPropertyBool( AI_CONFIG_IMPORT_FBX_READ_LIGHTS, false );
    importer.SetPropertyBool( AI_CONFIG_IMPORT_FBX_READ_CAMERAS, false );
    importer.SetPropertyBool( AI_CONFIG_IMPORT_FBX_READ_ALL_GEOMETRY_LAYERS, false );
    importer.SetPropertyBool( AI_CONFIG_FAVOUR_SPEED, true );

    // clang-format off
    unsigned int importFlags = 0
        | aiProcess_CalcTangentSpace
        | aiProcess_ValidateDataStructure
        | aiProcess_ImproveCacheLocality
        | aiProcess_RemoveRedundantMaterials
        | aiProcess_FindDegenerates
        | aiProcess_FindInvalidData
        | aiProcess_GenUVCoords
        | aiProcess_TransformUVCoords
        | aiProcess_FindInstances
        | aiProcess_GenSmoothNormals
        | aiProcess_Triangulate
        | aiProcess_FlipUVs
    ; // End of `importFlags` definition.
    // clang-format on

    GEP_ASSERT(importer.ValidateFlags(importFlags), "Invalid assimp import flags");

    const auto* pScene = importer.ReadFile(pFilename, importFlags);
    if (pScene == nullptr)
    {
        g_globalManager.getLogging()->logError("Error loading model from file '%s'", pFilename);
        throw LoadingError(importer.GetErrorString());
    }

    uint32 texturePathMemory = 0;
    uint32 materialNameMemory = 0;
    uint32 numNodes = 0;

    MemoryStatistics memstat;

    size_t modelDataSize = 0;

    uint32 numTextures = 0;

    uint32 nodeNameMemory = 0;

    uint32 numTextureReferences = 0;

    Load::Enum loadTexCoords [] = { Load::TexCoords0,
        Load::TexCoords1,
        Load::TexCoords2,
        Load::TexCoords3 };


    if( loadWhat & Load::Materials )
    {

        // Iterate over all materials to find all Textures
        for( size_t matIndex = 0; matIndex < pScene->mNumMaterials; matIndex++ )
        {
            aiMaterial* mat = pScene->mMaterials[ matIndex ];
            aiString texPath;
            if(mat->GetTexture( aiTextureType_DIFFUSE, 0, &texPath ) == aiReturn_SUCCESS)
            {
                // TODO: Check for duplicates?!
                texturePathMemory += texPath.length;
                ++numTextures;
                ++numTextureReferences;
            }
            else
            {
                g_globalManager.getLogging()->logWarning(
                    "Tried to load material without diffuse Texture, skipping..." );
            }

        }

        modelDataSize += sizeof( const char* ) * numTextures;
        memstat.texturePathReferencesMemory = MemoryPool( allocationSize<const char*>(numTextures) );
    }

    texturePathMemory += numTextures; // trailing \0 bytes
    texturePathMemory = static_cast< uint32 >(AlignmentHelper::__alignedSize( texturePathMemory ));
    if( loadWhat & Load::Materials )
    {
        modelDataSize += texturePathMemory;
        memstat.texturePathMemory = MemoryPool( texturePathMemory );


    }

    uint32 numBones = 0;
    uint32 numBoneInfos = 0;
    for( uint32 meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++ )
    {
        aiMesh* mesh = pScene->mMeshes[ meshIdx ];
        numBones += mesh->mNumBones;
        numBoneInfos += mesh->mNumVertices;

    }

    uint32 numMaterials = pScene->mNumMaterials;
    if( loadWhat & Load::Materials )
    {
        for( uint32 matIndex = 0; matIndex < numMaterials; matIndex++ )
        {
            aiString matName;
            pScene->mMaterials[ matIndex ]->Get( AI_MATKEY_NAME, matName );
            materialNameMemory += matName.length;
        }

        memstat.materialData = MemoryPool( numMaterials * sizeof( MaterialData ) );
        modelDataSize += memstat.materialData.size;

        materialNameMemory += numMaterials; /// trailing \0
        if( materialNameMemory % AlignmentHelper::__ALIGNMENT != 0 )
            materialNameMemory += AlignmentHelper::__ALIGNMENT - ( materialNameMemory % AlignmentHelper::__ALIGNMENT );

        modelDataSize += materialNameMemory;
        memstat.materialNameMemory = MemoryPool( materialNameMemory );
    }

    if( loadWhat & Load::Bones )
    {
        modelDataSize += sizeof( BoneNode ) * numBones;
        memstat.boneDataArray.size += sizeof( BoneNode ) * numBones;

        modelDataSize += sizeof( BoneInfo ) * numBoneInfos;
        memstat.boneDataArray.size += sizeof( BoneInfo ) * numBoneInfos; // extend the existing pool
    }

    uint32 numMeshes = pScene->mNumMeshes;
    if( loadWhat & Load::Meshes )
    {
        modelDataSize += sizeof( MeshData ) * numMeshes;
        memstat.meshDataArray = MemoryPool( sizeof( MeshData ) * numMeshes );
    }
    for( uint32 i = 0; i < numMeshes; i++ )
    {
        uint32 numVertices = 0, numComponents = 0, numTexcoords = 0;
        const aiMesh* mesh = pScene->mMeshes[ i ];
        numVertices = mesh->mNumVertices;

        if (mesh->HasPositions())
        {
            numComponents++;
        }
        if( ( mesh->HasNormals() ) && ( loadWhat & Load::Normals ) )
        {
            numComponents++;
        }
        if( ( mesh->HasTangentsAndBitangents() ) && ( loadWhat & Load::Tangents ) )
        {
            numComponents++;
        }
        if( ( mesh->HasTangentsAndBitangents() ) && ( loadWhat & Load::Bitangents ) )
        {
            numComponents++;
        }
        numTexcoords = mesh->GetNumUVChannels();

        modelDataSize += allocationSize<float>( numVertices * 3 ) * numComponents;
        memstat.vertexData.size += allocationSize<float>( numVertices * 3 ) * numComponents;

        for( uint32 j = 0; j < numTexcoords; j++ )
        {
            uint8 numUVComponents = mesh->mNumUVComponents[ j ];
            if( loadWhat & loadTexCoords[ j ] )
            {
                modelDataSize += allocationSize<float>( numVertices * 3 ) * numUVComponents;
                memstat.vertexData.size += allocationSize<float>( numVertices * 3 ) * numUVComponents;
            }
        }

        uint32 numFaces = mesh->mNumFaces; // AKA Index Buffer

        if( loadWhat & Load::Meshes )
        {
            modelDataSize += numFaces * sizeof( FaceData );
            memstat.faceDataArray.size += numFaces * sizeof( FaceData );
        }
    }

    uint32 numNodeReferences = 0, numMeshReferences = 0;
    numNodes = 1; // Root Node
    // Need to specify type because of recursion
    std::function<void( aiNode*, std::function<void( aiNode* )> )> traverseNodes = [ &]( aiNode* myNode, std::function<void( aiNode* )> nodeAction )
    {
        nodeAction( myNode );
        for( uint32 childIdx = 0; childIdx < myNode->mNumChildren; childIdx++ )
        {
            GEP_ASSERT( myNode->mChildren[ childIdx ] != nullptr, "Child was nullptr but parent said there would be a child!" );
            traverseNodes( myNode->mChildren[ childIdx ], nodeAction );
        }
    };

    traverseNodes( pScene->mRootNode, [ & ]( aiNode* myNode )
    {
        numNodes += myNode->mNumChildren;
        nodeNameMemory += myNode->mName.length;
        numMeshReferences += myNode->mNumMeshes;
        numNodeReferences += myNode->mNumChildren;
    } );

    nodeNameMemory += numNodes; // trailing \0 bytes

    if( nodeNameMemory % AlignmentHelper::__ALIGNMENT != 0 )
        nodeNameMemory += AlignmentHelper::__ALIGNMENT - ( nodeNameMemory % AlignmentHelper::__ALIGNMENT );

    if( loadWhat & Load::Nodes )
    {
        modelDataSize += numNodes * sizeof( NodeData );
        modelDataSize += numNodes * sizeof( NodeDrawData );
        memstat.nodeData = MemoryPool( numNodes * sizeof( NodeData ) + numNodes * sizeof( NodeDrawData ) );

        modelDataSize += allocationSize<uint32>( numMeshReferences );
        memstat.meshReferenceMemory = MemoryPool( allocationSize<uint32>( 1 ) * numMeshReferences );

        modelDataSize += numNodeReferences * sizeof( NodeData* );
        memstat.nodeReferenceMemory = MemoryPool( numNodeReferences * sizeof( NodeDrawData* ) );

        modelDataSize += nodeNameMemory;
        memstat.nodeNameMemory = MemoryPool( nodeNameMemory );
    }
    if( loadWhat & Load::Materials )
    {
        modelDataSize += numTextureReferences * sizeof( TextureReference );
        memstat.textureReferenceMemory = MemoryPool( numTextureReferences * sizeof( TextureReference ) );
    }

    m_pModelDataAllocator = GEP_NEW( m_pAllocator, StackAllocator )( true, modelDataSize, m_pAllocator );
    m_pStartMarker = m_pModelDataAllocator->getMarker();

    // Pre-allocate stuff
    {
        if( loadWhat & Load::Nodes )
        {
            memstat.nodeData += allocationSize<NodeDrawData>( numNodes );
            m_nodes = GEP_NEW_ARRAY( m_pModelDataAllocator, NodeDrawData, numNodes );

            memstat.boneDataArray += allocationSize<BoneNode>( numBones );

            m_modelData.bones = GEP_NEW_ARRAY( m_pModelDataAllocator, BoneNode, numBones );
        }
    }

    // Load textures
    {
        if( loadWhat & Load::Materials )
        {
            if( numTextures > 0 )
            {
                memstat.texturePathReferencesMemory += allocationSize<const char*>( numTextures );
                m_modelData.textures = GEP_NEW_ARRAY( m_pModelDataAllocator, const char*, numTextures );

                memstat.texturePathMemory += allocationSize<char>( texturePathMemory );
                char* textureNames = static_cast< char* >(m_pModelDataAllocator->allocateMemory( texturePathMemory ));
                uint32 curTexIndex = 0;
                uint32 curTexNameOffset = 0;
                for( size_t matIndex = 0; matIndex < pScene->mNumMaterials; matIndex++ )
                {
                    aiMaterial* mat = pScene->mMaterials[ matIndex ];
                    aiString texPath;
                    if( mat->GetTexture( aiTextureType_DIFFUSE, 0, &texPath ) == aiReturn_SUCCESS )
                    {
                        // TODO: Check for duplicates?!
                        memcpy( textureNames + curTexNameOffset, &texPath.data, texPath.length + 1 );
                        m_modelData.textures[ curTexIndex ] = textureNames + curTexNameOffset;

                        curTexIndex++;
                        curTexNameOffset += texPath.length + 1;
                    }

                }

                GEP_ASSERT( curTexNameOffset <= texturePathMemory, "Texture names needed more space than calculated!" );
            }
        }
    }
    // Materials
    {
        if( loadWhat & Load::Materials )
        {

            if( numMaterials > 0 )
            {
                memstat.materialData += allocationSize<MaterialData>( numMaterials );
                m_modelData.materials = GEP_NEW_ARRAY( m_pModelDataAllocator, MaterialData, numMaterials );

                ArrayPtr<char> materialNames;
                size_t curNamePos = 0;
                memstat.materialNameMemory += allocationSize<char>( materialNameMemory );
                materialNames = GEP_NEW_ARRAY( m_pModelDataAllocator, char, materialNameMemory );


                for( uint32 matIndex = 0; matIndex < numMaterials; ++matIndex )
                {
                    aiMaterial* assimpMat = pScene->mMaterials[ matIndex ];
                    MaterialData& gepMat = m_modelData.materials[ matIndex ];
                    aiString matName;
                    //read material name
                    // AI_MATKEY_XXX fills the first 3 parameters
                    if(assimpMat->Get( AI_MATKEY_NAME, matName ) == AI_SUCCESS)
                    {
                        memcpy( materialNames.getPtr() + curNamePos, matName.data, matName.length + 1 );
                        gepMat.name = materialNames.getPtr() + curNamePos;
                        curNamePos += matName.length + 1;
                    }
                    else
                    {
                        g_globalManager.getLogging()->logWarning(
                            "Material with index %d has no material name!", matIndex);
                    }


                    uint32 numTextures = assimpMat->GetTextureCount( aiTextureType_DIFFUSE );
                    aiString textureName;

                    memstat.textureReferenceMemory += allocationSize<TextureReference>( numTextures );
                    gepMat.textures = GEP_NEW_ARRAY( m_pModelDataAllocator, TextureReference, numTextures );


                    for( uint32 texIndex = 0; texIndex < numTextures; texIndex++ )
                    {
                        assimpMat->GetTexture( aiTextureType_DIFFUSE, texIndex, &textureName );
                        uint32 textureRefIndex = 0xFFFFFFFF;
                        for( uint32 texRefIdx = 0; texRefIdx < m_modelData.textures.length(); texRefIdx++ )
                        {
                            if (strcmp(textureName.data, m_modelData.textures[texRefIdx]) == 0)
                            {
                                textureRefIndex = texRefIdx;
                                break;
                            }
                        }
                        GEP_ASSERT( textureRefIndex != 0xFFFFFFFF,
                            "A texture was referenced which was not capture before!" );

                        gepMat.textures[ texIndex ].file = m_modelData.textures[ textureRefIndex ];
                        gepMat.textures[ texIndex ].semantic = TextureType::DIFFUSE;
                    }
                }
            }
        }
    }
    //Meshes
    {
        if (loadWhat & Load::Meshes)
        {
            memstat.meshDataArray += allocationSize<MeshData>( numMeshes );
            m_modelData.meshes = GEP_NEW_ARRAY( m_pModelDataAllocator, MeshData, numMeshes );



            for( uint32 meshIdx = 0; meshIdx < numMeshes; meshIdx++ )
            {
                auto& gepMesh = m_modelData.meshes[ meshIdx ];
                aiMesh* assimpMesh = pScene->mMeshes[ meshIdx ];

                uint32 numVertices = assimpMesh->mNumVertices;

                memstat.vertexData += allocationSize<vec3>( numVertices );
                gepMesh.vertices = GEP_NEW_ARRAY( m_pModelDataAllocator, vec3, numVertices );

                auto copyVec3Array = []( ArrayPtr<vec3> dst, const aiVector3D* const src )
                {
                    // Assuming equal component layout
                    if( sizeof( vec3 ) == sizeof( aiVector3D ) )
                    {
                        memcpy( dst.getPtr(), src, sizeof( vec3 ) * dst.length() );
                    }
                    else
                    {
                        for( uint32 vertexIdx = 0; vertexIdx < dst.length(); vertexIdx++ )
                        {
                            dst[ vertexIdx ].x = src[ vertexIdx ].x;
                            dst[ vertexIdx ].y = src[ vertexIdx ].y;
                            dst[ vertexIdx ].z = src[ vertexIdx ].z;
                        }
                    }

                };

                copyVec3Array( gepMesh.vertices, assimpMesh->mVertices );

                gepMesh.PerVertexFlags = 0;
                gepMesh.PerVertexFlags |= PerVertexData::Position;

                if (loadWhat & Load::Normals && assimpMesh->HasNormals())
                {
                    memstat.vertexData += allocationSize<vec3>( numVertices );
                    gepMesh.normals = GEP_NEW_ARRAY( m_pModelDataAllocator, vec3, numVertices );
                    copyVec3Array( gepMesh.normals, assimpMesh->mNormals);
                    gepMesh.PerVertexFlags |= PerVertexData::Normal;

                }

                if (loadWhat & Load::Tangents && assimpMesh->HasTangentsAndBitangents())
                {
                    memstat.vertexData += allocationSize<vec3>( numVertices );
                    gepMesh.tangents = GEP_NEW_ARRAY( m_pModelDataAllocator, vec3, numVertices );
                    copyVec3Array( gepMesh.tangents, assimpMesh->mTangents );
                    gepMesh.PerVertexFlags |= PerVertexData::Tangent;

                }

                if( loadWhat & Load::Bitangents && assimpMesh->HasTangentsAndBitangents() )
                {
                    memstat.vertexData += allocationSize<vec3>( numVertices );
                    gepMesh.bitangents = GEP_NEW_ARRAY( m_pModelDataAllocator, vec3, numVertices );
                    copyVec3Array( gepMesh.bitangents, assimpMesh->mBitangents );
                    gepMesh.PerVertexFlags |= PerVertexData::Bitangent;
                }

                if( loadWhat & ( Load::TexCoords0 | Load::TexCoords1 | Load::TexCoords2 | Load::TexCoords3 ) )
                {
                    uint32 numTexCoords = assimpMesh->GetNumUVChannels();
                    switch( numTexCoords )
                    {
                    case 4:
                        gepMesh.PerVertexFlags |= PerVertexData::TexCoord3;
                        // no break!
                    case 3:
                        gepMesh.PerVertexFlags |= PerVertexData::TexCoord2;
                        // no break!
                    case 2:
                        gepMesh.PerVertexFlags |= PerVertexData::TexCoord1;
                        // no break!
                    case 1:
                        gepMesh.PerVertexFlags |= PerVertexData::TexCoord0;
                        break;
                    }
                    for( uint32 texCoordIdx = 0; texCoordIdx < numTexCoords; texCoordIdx++ )
                    {
                        uint32 numUVComponents = assimpMesh->mNumUVComponents[ texCoordIdx ];
                        if (numUVComponents != 2)
                        {
                            std::ostringstream msg;
                            msg << "Currently only 2 component texture coordinates are supported got " << numUVComponents << " components";
                            throw LoadingError( msg.str() );
                        }
                        if (loadWhat & loadTexCoords[ texCoordIdx ])
                        {
                            memstat.vertexData += allocationSize<vec2>( numVertices );
                            gepMesh.texcoords[ texCoordIdx ] = GEP_NEW_ARRAY( m_pModelDataAllocator, vec2, numVertices );
                            for( uint32 vertexIndex = 0; vertexIndex < numVertices; vertexIndex++ )
                            {
                                gepMesh.texcoords[ texCoordIdx ][ vertexIndex ].x =
                                    assimpMesh->mTextureCoords[ texCoordIdx ][ vertexIndex ].x;
                                gepMesh.texcoords[ texCoordIdx ][ vertexIndex ].y =
                                    assimpMesh->mTextureCoords[ texCoordIdx ][ vertexIndex ].y;
                            }
                        }
                    }
                }

                uint32 numFaces = assimpMesh->mNumFaces;

                memstat.faceDataArray += allocationSize<FaceData>( numFaces );
                gepMesh.faces = GEP_NEW_ARRAY( m_pModelDataAllocator, FaceData, numFaces );

                for( uint32 faceIdx = 0; faceIdx < numFaces; faceIdx++ )
                {
                    if (assimpMesh->mFaces[faceIdx].mNumIndices == 3)
                    {
                        for( uint8 i = 0; i < 3; i++ )
                        {
                            gepMesh.faces[ faceIdx ].indices[ i ] = assimpMesh->mFaces[ faceIdx ].mIndices[ i ];
                        }
                    }
                    else
                    {
                        std::ostringstream msg;
                        msg << "Only Triangle Faces are supported in meshes! got face with " <<
                            assimpMesh->mFaces[ faceIdx ].mNumIndices <<
                            " vertices";
                        throw LoadingError( msg.str() );
                    }
                }

                if( loadWhat & Load::Bones )
                {
                    // Read bone infos


                    memstat.boneDataArray += allocationSize<BoneInfo>( numVertices );
                    gepMesh.boneInfos = GEP_NEW_ARRAY( m_pModelDataAllocator, BoneInfo, numVertices );

                    for(auto& info : gepMesh.boneInfos )
                    {
                        for( uint32 i = 0; i < BoneInfo::NUM_SUPPORTED_BONES; i++ )
                        {
                            // Set to maximum so we can identify if it already has been set.
                            // (Will be set to 0 when done to avoid index errors)
                            info.boneIds[ i ] = static_cast<uint32>( std::numeric_limits<
                                std::remove_all_extents< decltype( BoneInfo::boneIds ) >::type >::max() );
                            info.weights[ i ] = 0.0f;
                        }
                    }

                    // Read all bone infos
                    for( uint32 boneIdx = 0; boneIdx < assimpMesh->mNumBones; boneIdx++ )
                    {
                        aiBone* bone = assimpMesh->mBones[ boneIdx ];
                        for( uint32 boneWeightVertexIdx = 0; boneWeightVertexIdx < bone->mNumWeights; boneWeightVertexIdx++ )
                        {
                            aiVertexWeight& weight = bone->mWeights[ boneWeightVertexIdx ];
                            BoneInfo& gepBoneInfo = gepMesh.boneInfos[ weight.mVertexId ];
                            uint32 i = 0;
                            for( ; i < BoneInfo::NUM_SUPPORTED_BONES; i++ )
                            {
                                if( gepBoneInfo.boneIds[ i ] == static_cast< uint32 >( std::numeric_limits <
                                    std::remove_all_extents< decltype( BoneInfo::boneIds ) >::type > ::max() ) )
                                {
                                    break;
                                }
                            }
                            GEP_ASSERT( i < BoneInfo::NUM_SUPPORTED_BONES, "Too many bone infos for vertex %d", weight.mVertexId );
                            gepBoneInfo.boneIds[ i ] = boneIdx;
                            gepBoneInfo.weights[ i ] = weight.mWeight;


                        }
                    }

                    for( auto& info : gepMesh.boneInfos )
                    {
                        for( uint32 i = 0; i < BoneInfo::NUM_SUPPORTED_BONES; i++ )
                        {
                            if(info.boneIds[ i ] == static_cast<uint32>( std::numeric_limits <
                                std::remove_all_extents< decltype( BoneInfo::boneIds ) >::type > ::max() ) )
                            {
                                info.boneIds[ i ] = 0;
                                info.weights[ i ] = 0.0f;
                            }
                        }
                    }


                }


                //Read and Create MetaData

                gepMesh.materialIndex = assimpMesh->mMaterialIndex;


                // Calculate the bounding Box
                vec3 minPos = vec3(
                    std::numeric_limits<vec3::component_t>::max(),
                    std::numeric_limits<vec3::component_t>::max(),
                    std::numeric_limits<vec3::component_t>::max() );

                vec3 maxPos = vec3(
                    std::numeric_limits<vec3::component_t>::min(),
                    std::numeric_limits<vec3::component_t>::min(),
                    std::numeric_limits<vec3::component_t>::min() );

                for( uint32 vertIdx = 0; vertIdx < numVertices; vertIdx++ )
                {
                    for( uint8 compIdx = 0; compIdx < vec3::dimension; compIdx++ )
                    {
                        if (gepMesh.vertices[vertIdx].data[compIdx] < minPos.data[compIdx])
                        {
                            minPos.data[ compIdx ] = gepMesh.vertices[ vertIdx ].data[ compIdx ];
                        }
                        if( gepMesh.vertices[ vertIdx ].data[ compIdx ] > maxPos.data[ compIdx ] )
                        {
                            maxPos.data[ compIdx ] = gepMesh.vertices[ vertIdx ].data[ compIdx ];
                        }
                    }
                }
                maxPos += vec3( 0.01f, 0.01f, 0.01f );
                gepMesh.bbox = AABB( minPos, maxPos );


            }

        }
    }
    // Nodes
    {
        if( loadWhat & Load::Nodes )
        {
            memstat.nodeNameMemory += allocationSize<char>( nodeNameMemory );
            auto nodeNames = GEP_NEW_ARRAY( m_pModelDataAllocator, char, nodeNameMemory );

            size_t curNodeNamePos = 0;

            memstat.nodeData += allocationSize<NodeData>( numNodes );
            auto nodesData = GEP_NEW_ARRAY( m_pModelDataAllocator, NodeData, numNodes );

            uint32 curNodeIdx = 0;

            traverseNodes( pScene->mRootNode, [ &]( aiNode* node )
            {
                NodeData& gepNode = nodesData[ curNodeIdx ];
                memcpy( nodeNames.getPtr() + curNodeNamePos, node->mName.data, node->mName.length + 1 );
                gepNode.name = nodeNames.getPtr() + curNodeNamePos;
                curNodeNamePos += node->mName.length + 1;
                curNodeIdx++;
            } );

            curNodeIdx = 0;

            // Need to specify type because of recursion
            std::function<void( aiNode* )> traverseNodes = [ &]( aiNode* myNode )
            {
                NodeData& gepNode = nodesData[ curNodeIdx ];
                m_nodes[ curNodeIdx ].data = &nodesData[ curNodeIdx ];

                NodeDrawData& drawData = m_nodes[ curNodeIdx ];
                memstat.nodeReferenceMemory += allocationSize<NodeDrawData*>( myNode->mNumChildren );
                drawData.children = GEP_NEW_ARRAY( m_pModelDataAllocator, NodeDrawData*, myNode->mNumChildren );

                memstat.meshReferenceMemory += allocationSize<uint32>( myNode->mNumMeshes );
                drawData.meshes = GEP_NEW_ARRAY( m_pModelDataAllocator, uint32, myNode->mNumMeshes );

                static_assert( sizeof( gep::uint32 ) == sizeof( std::remove_pointer<decltype( myNode->mMeshes )>::type ),
                    "assimp uses something different than uint32 for mesh Indices" );
                memcpy( drawData.meshes.getPtr(), myNode->mMeshes, sizeof( uint32 ) * myNode->mNumMeshes );

                aiMatrix4x4& pData = myNode->mTransformation;
                mat4 f(DO_NOT_INITIALIZE);
                f.data[ 0 ] = pData.a1; f.data[ 1 ] = pData.a2; f.data[ 2 ] = pData.a3; f.data[ 3 ] = pData.a4;
                f.data[ 4 ] = pData.b1; f.data[ 5 ] = pData.b2; f.data[ 6 ] = pData.b3; f.data[ 7 ] = pData.b4;
                f.data[ 8 ] = pData.c1; f.data[ 9 ] = pData.c2; f.data[ 10 ] = pData.c3; f.data[ 11 ] = pData.c4;
                f.data[ 12 ] = pData.d1; f.data[ 13 ] = pData.d2; f.data[ 14 ] = pData.d3; f.data[ 15 ] = pData.d4;
                drawData.transform = f.transposed();

                curNodeIdx++;

                for( uint32 childIdx = 0; childIdx < myNode->mNumChildren; childIdx++ )
                {
                    drawData.children[ childIdx ] = &m_nodes[ curNodeIdx ];
                    GEP_ASSERT( myNode->mChildren[ childIdx ] != nullptr, "Child was nullptr but parent said there would be a child!" );
                    traverseNodes( myNode->mChildren[ childIdx ] );
                    drawData.children[ childIdx ]->parent = &drawData;

                }
            };

            traverseNodes( pScene->mRootNode );

            m_modelData.rootNode = &m_nodes[ 0 ];

        }

    }

    // Bones
    {
        if (loadWhat & Load::Bones)
        {
            uint32 curBoneIdx = 0;

            // Helper function for searching a node in the nodetree
            std::function< NodeDrawData*( NodeDrawData* myNode, const char* )> findNode =
                [ &]( NodeDrawData* myNode, const char* nodeName ) -> NodeDrawData*
            {
                if( strcmp( myNode->data->name, nodeName ) == 0 )
                {
                    return myNode;
                }
                else
                {
                    for( auto* child : myNode->children )
                    {
                        auto* result = findNode( child, nodeName );
                        if( result )
                        {
                            return result;
                        }
                    }
                }
                return nullptr;
            };

            for( uint32 meshIdx = 0; meshIdx < numMeshes; meshIdx++ )
            {
                auto& gepMesh = m_modelData.meshes[ meshIdx ];
                aiMesh* assimpMesh = pScene->mMeshes[ meshIdx ];

                if( !assimpMesh->HasBones() )
                {
                    continue;
                }

                uint32 numMeshBones = assimpMesh->mNumBones;

                gepMesh.rootBone = &m_modelData.bones[ curBoneIdx ];
                uint32 boneOffset = curBoneIdx;

                for( BoneInfo& info : gepMesh.boneInfos )
                {
                    for( uint32 weightIdx = 0; weightIdx < BoneInfo::NUM_SUPPORTED_BONES; weightIdx++ )
                    {
                        // Offset all bones by the previous created number of bones from other submeshes,
                        // so the mesh-local indices match our global indices
                        info.boneIds[ weightIdx ] += boneOffset;
                    }
                }

                for( uint32 boneIdx = 0; boneIdx < numMeshBones; boneIdx++ )
                {
                    aiBone* assimpBone = assimpMesh->mBones[ boneIdx ];
                    BoneNode& gepBone = m_modelData.bones[ curBoneIdx ];

                    bool duplicate = false;
                    uint32 duplicateIdx = 0;
                    for( uint32 bIdx=0; bIdx < curBoneIdx;++bIdx )
                    {
                        // Check if we already loaded this bone from a previous mesh
                        if (strcmp(assimpBone->mName.C_Str(), m_modelData.bones[bIdx].node->data->name) == 0)
                        {
                            duplicate = true;
                            duplicateIdx = bIdx;
                            break;

                        }
                    }
                    if( duplicate )
                    {
                        // Is our root bone a duplicate, then set it to the already loaded bone instead of the duplicated one
                        if( gepMesh.rootBone == &m_modelData.bones[curBoneIdx] )
                        {
                            gepMesh.rootBone = &m_modelData.bones[ duplicateIdx ];
                        }
                        // Correct the indices which point to the duplicated bone
                        // And correct also all subsequent bone indices which are now off by one
                        // because we skipped this bone
                        for( BoneInfo& info : gepMesh.boneInfos )
                        {
                            for( uint32 weightIdx = 0; weightIdx < BoneInfo::NUM_SUPPORTED_BONES; weightIdx++ )
                            {
                                // Set all bone weights of the duplicate to the index of the already loaded bone
                                if (info.boneIds[weightIdx] == curBoneIdx)
                                {
                                    info.boneIds[ weightIdx ] = duplicateIdx;
                                }
                                // Since we remove a bone from the list
                                // (because of duplication, decrement all bigger Indexes to close the gap
                                else if (info.boneIds[weightIdx] > curBoneIdx)
                                {
                                    info.boneIds[ weightIdx ]--;
                                }
                            }
                        }

                    }
                    // Not a duplicate, load the bone
                    else
                    {

                        aiMatrix4x4& pData = assimpBone->mOffsetMatrix;
                        mat4 f( DO_NOT_INITIALIZE );
                        f.data[ 0 ] = pData.a1; f.data[ 1 ] = pData.a2; f.data[ 2 ] = pData.a3; f.data[ 3 ] = pData.a4;
                        f.data[ 4 ] = pData.b1; f.data[ 5 ] = pData.b2; f.data[ 6 ] = pData.b3; f.data[ 7 ] = pData.b4;
                        f.data[ 8 ] = pData.c1; f.data[ 9 ] = pData.c2; f.data[ 10 ] = pData.c3; f.data[ 11 ] = pData.c4;
                        f.data[ 12 ] = pData.d1; f.data[ 13 ] = pData.d2; f.data[ 14 ] = pData.d3; f.data[ 15 ] = pData.d4;
                        gepBone.offsetMatrix = f.transposed();

                        // Find our related node in the hierarchy and link it.
                        NodeDrawData* drawData = findNode( m_modelData.rootNode, assimpBone->mName.C_Str() );
                        GEP_ASSERT( drawData, "Could not find node for bone!" );
                        gepBone.node = drawData;

                        curBoneIdx++;
                    }
                }

            }
            // We do waste (not leak!) a little bit of memory here,
            // because we initially did not check for duplicate bones in the meshes,
            // But we skipped the creation of those duplicate leaving sizeof(BoneNode) * numDuplicates unused memory behind
            // at the end of this array.
            // We need to truncate the stored array, otherwise the renderer thinks we have more bones than we actually have.
            if( curBoneIdx > 0 )
            {
                m_modelData.bones = m_modelData.bones( 0, curBoneIdx );
            }
        }
    }
    //Normalize and make texture paths relative
    {
        if (loadWhat & Load::Materials)
        {
            uint32 idx = 0;
            for( const char* tex : m_modelData.textures )
            {
                normalizePath( const_cast< char*>( tex ) );
                auto newPath = extractDataPath( ArrayPtr<char>( const_cast<char*>( tex ), strlen(tex) ) );

                if( newPath.getPtr() != nullptr )
                {
                    // Search for references in materials and replace it with the new pointer
                    for( auto& mat : m_modelData.materials )
                    {
                        for( auto& texRef : mat.textures )
                        {
                            if( texRef.file == tex )
                            {
                                texRef.file = newPath.getPtr();
                            }
                        }
                    }
                    m_modelData.textures[ idx ] = newPath.getPtr();
                }
                idx++;
            }


        }
    }

    g_globalManager.getLogging()->logMessage( "Loading of mesh %s took %f seconds", pFilename, timer.getTimeAsFloat() );

    m_modelData.hasData = true;

}

