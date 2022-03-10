#include "Parsing.hpp"
#include <sstream>
#include <ctime>
#include <sys/stat.h>

int		main(int argc, char **argv)
{
	/* { */
	/* 	if (argc < 2) { */
	/* 		std::cerr << "Usage: " << argv[0] << " path\n"; */
	/* 		return 1; */
	/* 	} */
	/* 	std::string path(argv[1]); */
	/* 	struct stat s; */
	/* 	if ( lstat(path.c_str(), &s) == 0 ) { */
	/* 		if ( S_ISDIR(s.st_mode) ) { */
	/* 			// it's a directory */
	/* 			std::cout << path << " is a dir.\n"; */
	/* 		} else if (S_ISREG(s.st_mode)) { */
	/* 			// it's a file */
	/* 			std::cout << path << " is a file.\n"; */
	/* 		} else if (S_ISLNK(s.st_mode)) { */
	/* 			// it's a symlink */
	/* 			std::cout << path << " is a symbolic link.\n"; */
	/* 		} else { */
	/* 			//something else */
	/* 			std::cout << path << " type is not recognized by this program.\n"; */
	/* 		} */
	/* 	} else { */
	/* 		//error */
	/* 		std::cerr << "stat() return !0 value\n"; */
	/* 		return 1; */
	/* 	} */
	/* 	return 0; */
	/* } */

	server_config	data;

	parsing(argc, argv, data);

	std::cout << data.server[0].location[0].path << std::endl;
	std::cout << data.server[0].location[0].root << std::endl;
	std::cout << data.server[0].error_page.first << std::endl;
	std::cout << data.server[0].error_page.second << std::endl;
	std::cout << data.server[0].location[1].client_max_body_size << std::endl;
	std::cout << data.server[0].location[0].client_max_body_size << std::endl;

	std::cout << data.server[1].server_name[0] << std::endl;
	return 0;
}
