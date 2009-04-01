#include "WebSite.h"
#include "FilePreprocess.h"
#include "Bitext.h"
#include "GlobalParams.h"
#include "DownloadMod.h"
#include <fstream>
#include <sstream>
#include <getopt.h>

using namespace std;


string
GetFilePath(string path)
{
	string exit;
	size_t found;

	found=path.find_last_of("/\\");
	exit=path.substr(0,found+1);

	return exit;
}

int
main (int argc, char *const *argv)
{
	WebSite *ws;
	vector<Bitext> results;
	unsigned int n_results;
	Bitext bitext;
	ifstream file;
	unsigned int i;
	string file_name;
	bool show_howtouse=false, mode_set=false;
	string dest_dir="";
	string config_file="/usr/local/etc/bitextor/conf/config.xml";
	bool download, any_bitext;
	struct stat my_stat;
	int next_op;
	unsigned short option; //Indicates if the user has introduced an option
	extern char *optarg;

	setlocale(LC_CTYPE, "");
	
	
	//Short options
	const char* const short_op = "d:w:hc:sl:";
	
	//Set of long options
	const struct option long_op[] =
	{
		{ "directory", 1, NULL, 'd'}, //Execution in direct mode
		{ "web-site", 1, NULL, 'w'}, //Execution in 2-steps mode with text length comparison
		{ "set-languages", 0, NULL, 's'}, //Execution in 2-setps mode with alignment distance text comparison
		{ "help", 0, NULL, 'h'},
		{ "config-file", 1, NULL, 'c'},
		{ "log-file", 1, NULL, 'l'},
		{ NULL, 0, NULL, 0}
	};
	
	next_op = getopt_long (argc, argv, short_op, long_op, NULL);
	for(option=0; next_op!=-1 && !show_howtouse; option++){
		switch(next_op){
			case 'd':
				if(mode_set){
					wcout<<"You only can select one option [-d|-w] to set the target of bitextor."<<endl;
					show_howtouse=true;
				}
				else{
					dest_dir=optarg;// option++;
					download=false;
					mode_set=true;
					if(dest_dir==""){
						wcout<<"You must define the directory from wich bitext must be extracted."<<endl;
						show_howtouse=true;
					}
				}
			break;
			case 'w':
				if(mode_set){
					wcout<<"You only can select one option [-d|-w] to set the target of bitextor."<<endl;
					show_howtouse=true;
				}
				else{
					dest_dir=optarg;// option++;
					download=true;
					mode_set=true;
					if(dest_dir==""){
						wcout<<"You must define the URL from wich bitext must be extracted."<<endl;
						show_howtouse=true;
					}
				}
			break;
			case 's':
				GlobalParams::SetGuessLanguage(false);
			break;
			case 'l':
				GlobalParams::OpenLog(optarg); //option++;
			break;
			case 'h': show_howtouse=true; break;
			case 'c': config_file=optarg; /*option++;*/ break;
			default:
				wcout<<"Unknown option "<<next_op<<"."<<endl;
				show_howtouse=true;
			break;
		}
		next_op = getopt_long (argc, argv, short_op, long_op, NULL);
	}

	if(show_howtouse || !mode_set)
		wcout<<L"The correct way to call Bitextor is:"<<endl<<L"\tbitextor [options] -w url"<<endl<<L"\tbitextor [options] -d local_directory_with_html_files"<<endl<<endl<<L"OPTIONS"<<endl<<L"\tTo see this use instructions, use the option -h"<<endl<<L"\tTo especify the path of the configuration file use the option -c path_of_configuration_file"<<endl<<L"\tTo set manually the language of every file, use the option --set_languages"<<endl;
	else{
		try{
			if(!GlobalParams::LoadGlobalParams(config_file))
				wcerr<<L"Bitextor can't open the config file. Please, specifie it with the option -c or place it at /usr/local/etc/bitextor/conf/"<<endl;
			else{
				if(download){
					DownloadMod mod;
					mod.SetDestPath(GlobalParams::GetDownloadPath());
					mod.StartDownload(Config::toWstring(dest_dir));
				}
				wcout<<L"Initializing Bitextor's destination path..."<<endl;
				if(dest_dir[dest_dir.length()-1]!='/')
					dest_dir+="/";
				if(stat((dest_dir+"bitexts/").c_str(), &my_stat) != 0)
					mkdir((dest_dir+"/bitexts/").c_str(),0777);
				wcout<<L"Comparing files and generating bitexts..."<<endl;
				ws=new WebSite(dest_dir);
				try{
					if(!ws->GenerateBitexts(dest_dir+"bitexts/"))
						wcout<<L"No correspondences were found between the files in the specified directory."<<endl;
				}
				catch(char const*e){
					cout<<e<<endl;
				}
				delete ws;
				Config::CleanUpConfiguration();
			}
		}
		catch(char* e){
			cout<<e<<endl;
		}
	}
	GlobalParams::CloseLog();

	return 0; 
}
