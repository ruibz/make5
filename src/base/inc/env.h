#ifndef ENV_H_
#define ENV_H_

#include <string>
#include <map>

class Env
{
public:
	Env();
	virtual ~Env();
public:
	std::string toString();
	bool update(const std::string &, const std::string &);
	void execute();
	void writeToFile(const std::string &);
	void addEnvFromText(const std::string &);
	void addEnvFromMain(char *envArray[]);
	std::string replaceByEnv(const std::string &);
	std::string getEnv(const std::string &);
public:
	static bool isExist(const std::string &);
private:
	std::map<std::string, std::string> _envMap;
};

class GlobalEnvMap
{
private:
	GlobalEnvMap();
public:
	~GlobalEnvMap();
public:
	static Env * getInstance();
};

std::string homeDir();
std::string userName();
std::string hostName();
std::string desktop();
std::string viewName();
std::string localViewDir();
std::string tmpDir();
std::string localTmpDir();
std::string homeLogDir();
std::string homeTmpDir();
bool isInVnc();
std::string make5ConfigFile();
std::string getMake5Env(const std::string &);
std::string domain();
std::string type();
std::string mode();
std::string lab();
std::string poolName();
bool isIc();
bool isUt();
bool isTest();
bool isDod();
std::string numOfDodMachine();
std::string site();
bool isXterm();
std::string make5RootDir();
std::string jobId();
int serviceMainPlugPort();
int nodeMainPort();
std::string serverIp();
void setAdditionalEnv();
std::string ipFile();
std::string remoteMachineTcl();
std::string nodeMain();
std::string hostnameToIpFile();
std::string runDir();
std::string runDataDir();
std::string downloadDir();
std::string priorityFile();
void moveToBackupFile(const std::string &);
bool isLincaseVersionIgnore();
std::string jobIdCreateNewFile();
bool optimizeCMakeNewWayFlag();
std::string priority();
int requestIpTimeout();
bool isCreateNewJobId();
bool isValgrind();
bool isTargetFileDoubleCheck();
bool isLocal();
bool isWinkin();
bool isAnalyze();
bool isSingleCapacity();
bool isExcludeMaster();

#endif /* ENV_H_ */
