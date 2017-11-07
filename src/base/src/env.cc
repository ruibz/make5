#include "env.h"
#include "funcString.h"
#include <iostream>
#include <stdlib.h>
#include "fileHandler.h"
#include "lock.h"
#include "funcContainer.h"
#include "job.h"

using namespace std;

Env::Env()
{
}

Env::~Env()
{
}

string Env::toString()
{
	return FuncMap<string, string> (_envMap).mapToString("=");
}

bool Env::update(const string &strName, const string &strValue)
{
	_envMap[strName] = strValue;

	return true;
}

void Env::execute()
{
	for (map<string, string>::iterator iterEnv = _envMap.begin(); iterEnv != _envMap.end(); iterEnv++)
	{
		string strName = iterEnv->first;
		string strValue = iterEnv->second;
		if (strName.empty() || strValue.empty())
			continue;
		setenv(strName.c_str(), strValue.c_str(), 1);
	}
}

void Env::writeToFile(const string & strFile)
{
	FileHandler(strFile).write(toString());
}

void Env::addEnvFromText(const string & strEnvData)
{
	vector<string> lines = FuncString(strEnvData).split("\n");
	for (vector<string>::iterator iter = lines.begin(); iter != lines.end(); iter++)
	{
		string strEnv = *iter;
		string strEnvName = FuncString(strEnv).subStrBefore("=");
		string strEnvValue = FuncString(strEnv).subStrAfter("=");
		_envMap[strEnvName] = strEnvValue;
	}
}

void Env::addEnvFromMain(char *envArray[])
{
	for (int i = 0; envArray[i] != NULL; i++)
	{
		string strEnv = envArray[i];
		if (!FuncString(strEnv).isFound("="))
			continue;
		string strEnvName = FuncString(strEnv).subStrBefore("=");
		string strEnvValue = FuncString(strEnv).subStrAfter("=");
		if (strEnvName.empty())
			continue;

		_envMap[strEnvName] = strEnvValue;
	}
}

string Env::replaceByEnv(const string & strData)
{
	string result(strData);

	for (map<std::string, std::string>::iterator iterMap = _envMap.begin(); iterMap != _envMap.end(); iterMap++)
	{
		string strEnvName = "$(" + iterMap->first + ")";
		string strValue = iterMap->second;
		if (FuncString(result).isFound(strEnvName))
			result = FuncString(result).replace(strEnvName, strValue);
	}

	return result;
}

string Env::getEnv(const string &strEnvName)
{
	if (_envMap.find(strEnvName) != _envMap.end())
	{
		return _envMap[strEnvName];
	}

	char *p = getenv(strEnvName.c_str());
	if (p == NULL)
		return "";
	return string(p);
}

bool Env::isExist(const string &strEnvName)
{
	char *p = getenv(strEnvName.c_str());
	if (p == NULL)
		return false;

	//	delete p;
	return true;
}

GlobalEnvMap::GlobalEnvMap()
{
}

GlobalEnvMap::~GlobalEnvMap()
{
}

CMutex envMapLock;
Env * GlobalEnvMap::getInstance()
{
	CMyLock lock(envMapLock);
	static Env * pEnvMap = NULL;
	if (pEnvMap == NULL)
	{
		pEnvMap = new Env();
	}

	return pEnvMap;
}

string homeDir()
{
	static string strHome = GlobalEnvMap::getInstance()->getEnv("HOME");
	return strHome;
}

string userName()
{
	static string strUser = GlobalEnvMap::getInstance()->getEnv("USER");
	return strUser;
}

std::string hostName()
{
	static string strEnv = GlobalEnvMap::getInstance()->getEnv("HOST");
	return strEnv;
}

string desktop()
{
	string strMake5Display = GlobalEnvMap::getInstance()->getEnv("make5_DISPLAY");
	if (!strMake5Display.empty())
		return strMake5Display;

	string desktop = GlobalEnvMap::getInstance()->getEnv("make5_desktop");
	if (!desktop.empty())
		return desktop;

	string strVncDeskTop = GlobalEnvMap::getInstance()->getEnv("VNCDESKTOP");
	if (!strVncDeskTop.empty())
	{
		string strDisplay = FuncString(strVncDeskTop).split(" ")[0];
		desktop = FuncString(strDisplay).trim();
		GlobalEnvMap::getInstance()->update("make5_desktop", desktop);
	}

	return desktop;
}

string viewName()
{
	string strView = GlobalEnvMap::getInstance()->getEnv("make5_view");
	if (!strView.empty())
		return strView;

	string strEnv = GlobalEnvMap::getInstance()->getEnv("CLEARCASE_CMDLINE");
	strView = FuncString(FuncString(strEnv).subStrAfter(" ")).trim();
	GlobalEnvMap::getInstance()->update("VIEW", strView);
	GlobalEnvMap::getInstance()->update("make5_view", strView);
	return strView;
}

string localViewDir()
{
	return "/local/" + userName() + "/" + viewName();
}

string tmpDir()
{
	string strTmpDir = GlobalEnvMap::getInstance()->getEnv("make5_tmpDir");
	if (!strTmpDir.empty())
		return strTmpDir;

	strTmpDir = homeDir() + "/make5_tmp/" + viewName();

	GlobalEnvMap::getInstance()->update("make5_tmpDir", strTmpDir);

	return strTmpDir;
}

string homeLogDir()
{
	string strDir = GlobalEnvMap::getInstance()->getEnv("make5_homeLogDir");
	if (!strDir.empty())
		return strDir;

	strDir = homeDir() + "/make5_log";

	GlobalEnvMap::getInstance()->update("make5_homeLogDir", strDir);

	return strDir;
}

string homeTmpDir()
{
	string strDir = GlobalEnvMap::getInstance()->getEnv("make5_homeTmpDir");
	if (!strDir.empty())
		return strDir;

	strDir = homeDir() + "/make5_tmp";

	GlobalEnvMap::getInstance()->update("make5_homeTmpDir", strDir);

	return strDir;
}

string localTmpDir()
{
	string strTmpDir = GlobalEnvMap::getInstance()->getEnv("make5_tmpDir");
	if (!strTmpDir.empty())
		return strTmpDir;

	strTmpDir = "/tmp/" + userName();

	GlobalEnvMap::getInstance()->update("make5_tmpDir", strTmpDir);

	return strTmpDir;
}

bool isInVnc()
{
	string strVncXstartUp = homeDir() + "/.vnc/xstartup";
	if (!FileHandler(strVncXstartUp).isExist())
		return false;

	string strData = FileHandler(strVncXstartUp).read();
	if (FuncString(strData).isFound("\twm &\n"))
		return false;

	if (("ALU_LANNION" == site() || "ALU_CTF" == site()) && domain() == "modem.l2")
		return false;

	string strVncDeskTop = GlobalEnvMap::getInstance()->getEnv("VNCDESKTOP");
	return !strVncDeskTop.empty();
}

string make5ConfigFile()
{
	string strConfigFile = GlobalEnvMap::getInstance()->getEnv("make5_configFile");
	if (!strConfigFile.empty())
		return strConfigFile;

	strConfigFile = tmpDir() + "/config.txt";
	GlobalEnvMap::getInstance()->update("make5_configFile", strConfigFile);
	return strConfigFile;
}

std::string getMake5Env(const std::string & strEnvName)
{
	string strResult = GlobalEnvMap::getInstance()->getEnv(strEnvName);
	if (!strResult.empty())
		return strResult;

	return strEnvName + "_unknown";
}

string domain()
{
	return getMake5Env("make5_domain");
}

string type()
{
	return getMake5Env("make5_type");
}

string mode()
{
	return getMake5Env("make5_mode");
}

string lab()
{
	return getMake5Env("make5_lab");
}

string poolName()
{
	return getMake5Env("make5_poolName");
}

bool isIc()
{
	static bool result = ("yes" == getMake5Env("make5_ic"));
	return result;
}

bool isUt()
{
	static bool result = ("yes" == getMake5Env("make5_ut"));
	return result;
}

bool isTest()
{
	static bool result = ("yes" == getMake5Env("make5_test"));
	return result;
}

bool isDod()
{
	static bool result = ("yes" == getMake5Env("make5_dod"));
	return result;
}

string numOfDodMachine()
{
	return GlobalEnvMap::getInstance()->getEnv("make5_numOfDodMachine");
}

string site()
{
	return GlobalEnvMap::getInstance()->getEnv("SWE_SITE");
}

bool isXterm()
{
	//defensive
	return getMake5Env("make5_xterm") != "no";
}

string make5RootDir()
{
	return getMake5Env("make5_rootDir");
}

string jobId()
{
	return GlobalEnvMap::getInstance()->getEnv("make5_jobId");
}

int serviceMainPlugPort()
{
	return (5100 + 5 * string2int(jobId()));
}

int nodeMainPort()
{
	return serviceMainPlugPort() + 1;
}

string serverIp()
{
	string strServerIp = GlobalEnvMap::getInstance()->getEnv("make5_serverIp");
	if (!strServerIp.empty())
		return strServerIp;

	strServerIp = "172.24.220.85";

	map<string, string> siteIpMap;
	siteIpMap["ALU_SHANGHAI_JQ"] = "172.24.220.85";
	siteIpMap["ALU_LANNION"] = "172.24.220.85";

	string strSite = site();
	if (!strSite.empty())
		if (siteIpMap.find(strSite) != siteIpMap.end())
			strServerIp = siteIpMap[strSite];

	string strFile = homeDir() + "/make5_server_ip.txt";
	if (FileHandler(strFile).isExist())
		strServerIp = FuncString(FileHandler(strFile).read()).trim();

	GlobalEnvMap::getInstance()->update("make5_serverIp", strServerIp);
	return strServerIp;
}

void setAdditionalEnv()
{
	Env * pEnv = GlobalEnvMap::getInstance();
	pEnv->update("VIEW", viewName());
	pEnv->update("DESKTOP", desktop());
	//	pEnv->update("SERVER_IP", serverIp());
	pEnv->update("make5_serverIp", serverIp());
}

string ipFile()
{
	string strIpFile = GlobalEnvMap::getInstance()->getEnv("make5_ipFile");
	if (!strIpFile.empty())
		return strIpFile;

	strIpFile = homeDir() + "/make5_slave_ip_config.txt";
	if (FileHandler(strIpFile).isExist())
	{
		GlobalEnvMap::getInstance()->update("make5_ipFile", strIpFile);
		return strIpFile;
	}

	strIpFile = homeDir() + "/userConfig.ini";
	GlobalEnvMap::getInstance()->update("make5_ipFile", strIpFile);
	return strIpFile;
}

string remoteMachineTcl()
{
	return make5RootDir() + "/app/common/remoteMachine.tcl";
}

string nodeMain()
{
	return make5RootDir() + "/main/make5_node_main";
}

string hostnameToIpFile()
{
	return make5RootDir() + "/config/common/Hostname_Ip.txt";
}

string runDir()
{
	//return homeDir() + "/run";
	return "/local/run";
}

string runDataDir()
{
	return runDir() + "/data";
}

string downloadDir()
{
	return runDataDir() + "/shutdown";
}

std::string priorityFile()
{
	return GlobalEnvMap::getInstance()->getEnv("make5_file_priority");
}

void moveToBackupFile(const string &strFileName)
{
	string strCommand = "mv " + strFileName + " " + strFileName + ".bak";
	if (!Job::systemCall(strCommand))
		cout << "Error: Command can't be executed: " << strCommand << endl;
}

bool isLincaseVersionIgnore()
{
	string strLincaseVersionIgnore = GlobalEnvMap::getInstance()->getEnv("make5_lincaseVersion_ignore");
	return (strLincaseVersionIgnore == "ignore");
}

std::string jobIdCreateNewFile()
{
	return tmpDir() + "/jobIdCreateNewFile";
}

bool optimizeCMakeNewWayFlag()
{
	string strOptimizeCMakeNewWayFlag = GlobalEnvMap::getInstance()->getEnv("make5_optimizeCMakeNewWayFlag");
	return (strOptimizeCMakeNewWayFlag == "yes");
}

string priority()
{
	return getMake5Env("make5_priority");
}

int requestIpTimeout()
{
	int timeout = string2int(getMake5Env("make5_requestIpTimeout"));
	if (timeout <= 0)
		timeout = 5;
	return timeout;
}

bool isCreateNewJobId()
{
	return "yes" == getMake5Env("make5_newJobId");
}

bool isValgrind()
{
	return "yes" == getMake5Env("make5_valgrind");
}

bool isTargetFileDoubleCheck()
{
	static bool result = ("yes" == getMake5Env("make5_isTargetFileDoubleCheck"));
	return result;
}

bool isLocal()
{
	return "yes" == getMake5Env("make5_local");
}

bool isWinkin()
{
	return "yes" == getMake5Env("make5_winkin");
}

bool isAnalyze()
{
	return "yes" == getMake5Env("make5_analyze");
}

bool isSingleCapacity()
{
	return "yes" == getMake5Env("make5_singleCapacity");
}

bool isExcludeMaster()
{
	return "yes" == getMake5Env("make5_excludeMaster");
}
