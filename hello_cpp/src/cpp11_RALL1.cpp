/*
 * cpp11_RALL1.cpp
 *
 *  Created on: Sep 20, 2017
 *      Author: tingqian
 */
#include <iostream>
#include <memory>
#include <cstdio>
#include <cstring>


void list_dir(const char * dir_name, const char * prefix);


/* ================================================================
 * IF all your resource managing class is doing the RAll way
 * you don't have to worry about return(including throw exception)
 * prematurely, everything is much easier.
 *
 * 1.write your own RAll class
 * 2.use smart pointer for light-weight solution to manage C/system resources
 * ================================================================
 */

class file
{
public:
	file(void): fp(nullptr), fpath(nullptr){
	}
	~file(void){
		printf("detor: [%s] closed\n", fpath);
		close();
	}
	void open(const char * path, const char * mode){
		close();
		fp = fopen(path, mode);
		fpath = path;
	}
	void close(void){
		if(fp) fclose(fp);
	}

private:
	FILE *fp;
	const char * fpath;
};


void test(void)
{
	std::shared_ptr<char> buf( new char[1024],
								[](char *p){delete []p; printf("buf is deleted by share_ptr\n");});
	std::shared_ptr<FILE> f1(fopen("Makefile","rb"),fclose);
							 //[](FILE *fp){if(fp) fclose(fp);printf("f3 is closed by shared_ptr\n");});
	file f2,f3;

	// we have to use get() to gain the actuall pointer type to an array.
	printf("fgets(f1) returns: %s", fgets(buf.get(), 1024, f1.get()));

	f1.reset(fopen("cpp11_RALL1","rb"), fclose);

	f2.open("Makefile","rb");

	list_dir("/dev/fd",">>> Opened Files:");

	throw 1;
	f3.open("Makefile2","rb");
}



int main()
{
	try{
		test();
	}catch(int k){

		printf(R"(
 ============Exeception is caught ===================
 We are here under exception, means we want an opportunity
 to continue w/o resource leakage, even if finally we still
 going to abort with throw.
 
   RALL is the right way to do so when we return prematurely.
 Stack unwinding is what C++ offer when we return by throw an exception.
 ====================================================
)");

		list_dir("/dev/fd",">>> Opened Files:");
		std::cout << "Error codeL" << k << std::endl;

		throw;
	}

}





//=========================================================
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
void list_dir(const char * dir_name, const char * prefix)
{
	DIR * pdir;
	struct dirent * ent;
	struct stat st;
	char strpath[1024];
	char symlink[1024];
	int n;

	pdir = opendir(dir_name);

	while((ent = readdir(pdir)) != NULL)
	{
		sprintf(strpath,"%s/%s", dir_name, ent->d_name);

		if(lstat(strpath, &st) == -1)
			perror("stat error"), exit(1);

		if(S_ISLNK(st.st_mode))
		{
			if((n = readlink(strpath, symlink, sizeof(symlink))) == -1)
				perror("readlink error"), exit(1);

			if(n > 0)
				symlink[n] = 0;
			else
				strcpy(symlink,"?");
			printf("%s%s -> %s\n",prefix, strpath, symlink);
		}
		else
			printf("%s%s\n", prefix,strpath);
	}

	closedir(pdir);
}


