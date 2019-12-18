#ifndef __SYM_LOGGER
#define __SYM_LOGGER

#include <iostream>
#include <fstream>
#include <string>

namespace sym
{
    class CLogger
    {
        public:
			CLogger();
			~CLogger();

            //Open and clear log file
			bool Init(std::string FName);

            //Close log file
            void Close();

            //Add log entry
			void AddEntry(std::string Text);

        private:
            //Log filename
			std::string Filename;

            //File
            std::ofstream File;

            //Get time stamp
			std::string GetTimestamp(std::string Format);
    };

} //Namespace

#endif // __SYM_LOGGER
