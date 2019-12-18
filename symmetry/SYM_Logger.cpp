#include <time.h>

#include "SYM_Logger.h"
#include "SYM_Engine.h"

namespace sym
{
	CLogger::CLogger()
	{
	}

	CLogger::~CLogger()
	{
		if(File.is_open()) File.close();
	}

    bool CLogger::Init(std::string FName)
    {
        //Prefix filename with path
        Filename = Engine->SystemPath + FName;

        //Open file for write, remove existing data
		File.open(Filename.c_str(), std::ios::out | std::ios::trunc);

		//Check if file exists
		if (!File.is_open())
		{
			std::cout << "Error: Could not create log file " + Filename << std::endl;
			return false;
		}

		//Add first log entry
		AddEntry("Log opened on " + GetTimestamp("%c"));

		return true;
    }

    void CLogger::Close()
    {
        //Close file
        File.close();
    }

    void CLogger::AddEntry(std::string Text)
    {
        //Timestamp the message
        Text = GetTimestamp("%X") + " :: " + Text;

        //Output the message
        std::cout << Text << std::endl;

        //If log file is open
        if(File.is_open())
        {
            //Add entry to file
            File << Text << "\n";

			//Flush the log, so no content gets lost in a segfault
			File.flush();
        }
        else
        {
            //Output warning
            std::cout << "Warning: Log entry added before log file opened" << std::endl;
        }
    }

    std::string CLogger::GetTimestamp(std::string Format)
    {
        //Get current time
        time_t RawTime;
        struct tm *TimeInfo;
        char Buffer [80];

        time(&RawTime );
        TimeInfo = localtime (&RawTime);

        //Get timestamp
        strftime (Buffer, 80, Format.c_str(), TimeInfo);

        return Buffer;
    }

} //Namespace
