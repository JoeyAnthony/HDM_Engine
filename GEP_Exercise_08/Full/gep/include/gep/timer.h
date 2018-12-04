#pragma once

namespace gep
{
	class PointInTime;
    /// \brief for measuring time
    class GEP_API Timer
    {
		friend PointInTime;
    private:
		LARGE_INTEGER performanceCount;
		double resolution = 0;
		double timeValue = 0;
		bool isPaused = false;

    public:
        Timer();

        /// \brief returns the time elapsed since the start of the timer as a float (in seconds)
        float getTimeAsFloat() const;
        /// \brief returns the time elapsed since the start of the timer as a double (in seconds)
        double getTimeAsDouble() const;
        /// \brief returns the time elapsed since the start of the timer as a 64 bit int (in the smallest measurable unit)
        uint64 getTime() const;
        /// \brief pauses the timer
        void pause();
        /// \brief continues the timer after pausing
        void unpause();
		/// \brief calculates time
		double calcTime() const;
    };

    /// \brief stores a point in time with maximum percision
    class GEP_API PointInTime
    {
    private:
		double resolution = 0;
		double timeValue = 0;
    public:
        /// \brief constructor
		PointInTime();
        /// \param timer the timer to use for measuring time
        PointInTime(Timer& timer);

        /// \brief computes the time difference between two points in time
        float operator - (const PointInTime& rh) const;

        // comparison operators
        bool operator > (const PointInTime& rh) const;
        bool operator < (const PointInTime& rh) const;
        bool operator >= (const PointInTime& rh) const;
        bool operator <= (const PointInTime& rh) const;
        bool operator == (const PointInTime& rh) const;
        bool operator != (const PointInTime& rh) const;
    };
}
