#pragma once

namespace gep
{
    /// \brief for measuring time
    class GEP_API Timer
    {
		LARGE_INTEGER frequency;

    public:
		Timer();

		float getTimeAsFloat() const;

		double getTimeAsDouble() const;

		gep::uint64 getTime() const;

		void pause();

		void unpause();


		
    };

    /// \brief stores a point in time with maximum percision
    class GEP_API PointInTime
    {
		Timer timer;

	public:
		PointInTime(Timer t);

		PointInTime(Timer& timer);

		float operator - (const PointInTime& rh) const;

		bool operator < (const PointInTime& rh) const;

		bool operator > (const PointInTime& rh) const;

		bool operator <= (const PointInTime& rh) const;

		bool operator >= (const PointInTime& rh) const;

		bool operator == (const PointInTime& rh) const;

		bool operator != (const PointInTime& rh) const;

    };
}
