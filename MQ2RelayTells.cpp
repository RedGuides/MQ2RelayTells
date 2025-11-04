//  Please Do Not Distribute Without Authors Consent.

/* MQ2Relaytells.cpp v3.1000
Plugin created by Sadge
Please do not distribute without my consent.
*************************************************************************************
| This is a simple plugin created to relay every incoming tell to EQBCS.         |
| In the MQ2Relaytells.ini file, you can change ChatChannel=/bc (default) to      |
| anything you want.  For example, if you wanted to relay the tells to a character   |
| named Sillytoon, you'd change it to ChatChannel=/tell Sillytoon.               |
*************************************************************************************

*****Version 3.1000*****
-- Added the ability to not relay custom tells (ie. plat spammers)
-- Usage: /relaycustom <text> will add <text> to your ini file and will not relay any tells that contain <text>.
*****Version 3.0210*****
-- Went back and added UserName and Password for mailrelay (if your smtp requires auth & you don't want to/can't install blat correctly
*****Version 3.0200*****
-- Finally fixed the guild treasurer and katta vendors from relaying if NPCRelays are off
*****Version 3.0100*****
-- Added RelayTellsInGame to ini file.  Setting this to FALSE will stop relays to the chat channel (you can still relay to mail with this set to false)
*****Version 3.0000*****
-- Added in the ability to relay tells to mail/cellphone using Blat 262 (www.blat.net)
-- Added /relaymailhelp to see current settings for mail relay
-- Thank you to "jamie75" for the suggestion to relay to mail
-- Thank you to "steelers" for pointing me to blat to make this simple once I set it up

*****Before Version 3.0000*****
-- Added help display for Trader. Also added a timestamp (thanks to simkin for the suggestion).
-- Added check for Traders.
-- Fixed issue with receiving a tell from someone with a familiar (would think the tell came from pet)
-- Added relay for ;tell in addition to the already relayed /tell (results in "told you" instead of "tells you")
-- Added ini file settings so you can now choose to relay pet messages, including attack, suspend, un-suspend, and "unable to wake"
-- Added an ini option to relay AFK messages.
-- Added the /relayhelp command. This allows you to view current settings
-- Added the /relayload command. This allows you to reload the ini file if you make changes.
-- Added NPC check (this will now not relay bank broker and vendor spam)
-- Added character-based ini files. Thanks to jamie75 for the request.
*/

#include <mq/Plugin.h>

PreSetup("MQ2Relaytells");
PLUGIN_VERSION(3.1);

char ChatChannel[MAX_STRING];               //Defines where to output the tell
char ChannelMessage[MAX_STRING];            //Defines what the output message is
char ChatSender[MAX_STRING];               //Defines who sent the tell
char IncomingText[MAX_STRING];               //Defines the completed output command
char TellText[MAX_STRING];                  //Defines what is the actual tell
char EmailTo[MAX_STRING];                  //Defines who to email
char EmailFrom[MAX_STRING];                  //Defines who is sending the email
char Subject[MAX_STRING];                  //Defines what the subject of the email is
char Server[MAX_STRING];                  //Defines what is your smpt server
char Body[MAX_STRING];                     //Defines the body of the email to send
char UserName[MAX_STRING];                  //Defines the body of the email to send
char Password[MAX_STRING];                  //Defines the body of the email to send
char Tell[20][MAX_STRING];

bool RelayAFKMessage = false;
bool RelayPetTells = true;
bool RelayPetAttack = true;
bool RelayPetSuspend = false;
bool RelayPetMezzed = false;
bool RelayNPCTells = false;
bool RelayTraderTells = false;
bool RelayTellsInGame = false;
bool RelayToMail = false;

int TotalCustomTells = 0;

void Load_INI(VOID);
void RelayHelp(PSPAWNINFO pChar, PCHAR szLine);
void RelayMailHelp(PSPAWNINFO pChar, PCHAR szLine);
void Reload_INI(PSPAWNINFO pChar, PCHAR szLine);
void MailRelay(VOID);
void AddCustomTell(PSPAWNINFO pChar, PCHAR szLine);
void RTSetKey(PSPAWNINFO pChar, PCHAR szLine);

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	AddCommand("/relayhelp", RelayHelp);
	AddCommand("/relaymailhelp", RelayMailHelp);
	AddCommand("/relayload", Reload_INI);
	AddCommand("/relaycustom", AddCustomTell);
	AddCommand("/rtsetkey", RTSetKey);

	DebugSpewAlways("Initializing MQ2Relaytells");
	WriteChatf("%s::Version [\ag%1.4f\ax] Loaded! Created by Sadge", mqplugin::PluginName, MQ2Version);
	WriteChatf("\ay====================\ax");
	WriteChatColor("You can view the current settings by typing /relayhelp", CONCOLOR_GREEN);
	WriteChatColor("You can view the mail relay settings by typing /relaymailhelp", CONCOLOR_GREEN);
	WriteChatColor("You can reload the ini file by typing /relayload", CONCOLOR_GREEN);
	WriteChatf("\ay====================\ax");
	if (GetGameState() == GAMESTATE_INGAME) {
		if (pLocalPC) {
			sprintf_s(INIFileName, "%s\\MQ2Relaytells_%s_%s.ini", gPathConfig, pLocalPC->Name, GetServerShortName());
			Load_INI();
		}
	}
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2Relaytells");
	RemoveCommand("/relayhelp");
	RemoveCommand("/relaymailhelp");
	RemoveCommand("/relayload");
	RemoveCommand("/relaycustom");
	RemoveCommand("/rtsetkey");
}

VOID ReportTell(PCHAR Line)
{
	//Actually reports the tell in whatever chat specified in the .ini file
	if (RelayTellsInGame) EzCommand(Line);
	if (RelayToMail) MailRelay();
}

void MailRelay(VOID) {
	char mailcommand[MAX_STRING];
	sprintf_s(mailcommand, "/system blat -t %s -f %s -s \"%s\" -server %s -body \"%s\" -u %s -pw %s", EmailTo, EmailFrom, Subject, Server, Body, UserName, Password);
	EzCommand(mailcommand);

}
std::string StringToLower(std::string strToConvert)
{//change each element of the string to lower case
	for (unsigned int i = 0; i<strToConvert.length(); i++)
	{
		strToConvert[i] = tolower(strToConvert[i]);
	}
	return strToConvert;//return the converted string
}
bool FilterCustomTells(PCHAR Line) {   //checks to see if incoming tell contains custom filtered text

	std::string IncomingTellLine;
	std::string CustomTellLine;
	size_t found;

	for (int i = 1; i<(TotalCustomTells + 1); i++) {
		IncomingTellLine = Line;
		IncomingTellLine = StringToLower(IncomingTellLine);
		CustomTellLine = Tell[i];
		CustomTellLine = StringToLower(CustomTellLine);

		found = IncomingTellLine.find(CustomTellLine);
		if (found != std::string::npos)
			return true;
	}
	return false;
}
void GetCustomTells(VOID) {   //Loads custom text strings from ini

	int MaxCustomTells = 20;
	char szTemp[MAX_STRING];

	TotalCustomTells = 0;

	for (int i = 1; i<(MaxCustomTells + 1); i++) {
		sprintf_s(szTemp, "Tell%i", i);

		if (!GetPrivateProfileString("CustomTells", szTemp, "", Tell[i], MAX_STRING, INIFileName))
			return;
		else
			TotalCustomTells++;
	}

}

void DoTellRelay(PCHAR Line)
{
	char NPCNameCheck[MAX_STRING];
	char MerchantNameCheck[MAX_STRING];
	char PCNameCheck[MAX_STRING];
	char PetNameCheck[MAX_STRING];
	char TraderNameCheck[MAX_STRING];
	char nullstring[] = "NULL";
	int TellLength;
	int length;

	char * pch;                           //This part gets the name of the tell sender
	pch = strchr(Line, ',');
	int i;
	i = (int)(pch - Line - 10);
	strcpy_s(ChatSender, "");
	strncat_s(ChatSender, Line, i);
	i = (int)(pch - Line + 4);                        //This part looks for the tell message
	TellLength = (int)strlen(Line) - 1;
	std::string str(Line);
	length = (int)str.copy(TellText, TellLength - i + 1, i - 1);
	TellText[length] = '\0';
	sprintf_s(IncomingText, "%s [${Time}] %s %s %s", ChatChannel, ChatSender, ChannelMessage, TellText);   //Sets the output for In Game relay
	sprintf_s(Body, "[${Time}] %s %s %s", ChatSender, ChannelMessage, TellText);   //Sets the output for Mail relay

	if (FilterCustomTells(Line)) return;

	sprintf_s(NPCNameCheck, "${Spawn[npc %s]}", ChatSender);            //Sets temptext for use to return TLO for NPC check
	ParseMacroData(NPCNameCheck, sizeof(NPCNameCheck));
	sprintf_s(MerchantNameCheck, "${Spawn[merchant %s]}", ChatSender);   //Sets temptext for use to return TLO for merchant check
	ParseMacroData(MerchantNameCheck, sizeof(MerchantNameCheck));
	sprintf_s(PetNameCheck, "${Spawn[pet %s]}", ChatSender);            //Sets temptext for use to return TLO for Pet check
	ParseMacroData(PetNameCheck, sizeof(PetNameCheck));
	sprintf_s(PCNameCheck, "${Spawn[pc %s]}", ChatSender);            //Sets temptext for use to return TLO for PC check (used to check for familiars)
	ParseMacroData(PCNameCheck, sizeof(PCNameCheck));
	sprintf_s(TraderNameCheck, "${Spawn[trader %s]}", ChatSender);      //Sets temptext for use to return TLO for Trader check
	ParseMacroData(TraderNameCheck, sizeof(TraderNameCheck));

	if (strcmp(NPCNameCheck, nullstring) != 0) {                     // NPC with name found?
		if (RelayNPCTells) ReportTell(IncomingText);
	}
	if (strcmp(ChatSender, "a guild treasurer") == 0) {               // Check for guild treasurer
		if (RelayNPCTells) {
			WriteChatf("Chat sender: %s", ChatSender);
			ReportTell(IncomingText);
		}
	}
	if (!_strnicmp(ChatSender, "Trader", 6)) {               // Merchant with name found?
		if (RelayNPCTells) ReportTell(IncomingText);
	}

	if ((strcmp(NPCNameCheck, nullstring) == 0) && (strcmp(ChatSender, "a guild treasurer") != 0) && (_strnicmp(ChatSender, "Trader", 6))) {               // If temptext=NULL (no NPC with name found), this will go relay text.
		if (strcmp(PetNameCheck, nullstring) != 0) {            // Pet with name found?
			if (strcmp(PCNameCheck, nullstring) != 0) {         // PC with name found same as pet? (familiar check)
				if (strcmp(TraderNameCheck, nullstring) != 0) {   // Trader with name found (and has a familiar up)
					if (RelayTraderTells) ReportTell(IncomingText);
				}
				else ReportTell(IncomingText);
			}
			if (RelayPetTells) {                     // Relay Pet tells?
				if (strstr(Line, "Attacking ")) {         // Attacking message (from pet)
					if (RelayPetAttack) ReportTell(IncomingText);
				}
				if (strstr(Line, "I am unable to wake ")) {      // Mezzed message (from pet)
					if (RelayPetMezzed) ReportTell(IncomingText);
				}
				if (strstr(Line, "By your command, master.")) {      // Suspended message
					if (RelayPetSuspend) ReportTell(IncomingText);
				}
			}
		}

		if (strcmp(PetNameCheck, nullstring) == 0) {         // No Pet with name found?
			if ((strstr(Line, "I live again...")) || (strstr(Line, "AFK Message:")) || (strstr(Line, "A.F.K."))) {      // Suspended message (used because no pet spawn found when unsuspending pet)
				if (strstr(Line, "I live again...")) {
					if (RelayPetSuspend) ReportTell(IncomingText);
				}
				if ((strstr(Line, "AFK Message:")) || (strstr(Line, "A.F.K."))) {      // AFK message?
					if (RelayAFKMessage) ReportTell(IncomingText);
				}
			}
			else {
				if (strcmp(TraderNameCheck, nullstring) != 0) {   // Trader with name found
					if (RelayTraderTells) ReportTell(IncomingText);
				}
				else ReportTell(IncomingText);      //All other checks passed means it's a standard tell and should be relayed
			}
		}
	}
}
bool BoolFromINI(const char* Section, const char* Key)
{
	bool value = GetPrivateProfileBool(Section, Key, false, INIFileName);
	WritePrivateProfileBool(Section, Key, value, INIFileName);
	return value;
}

void Load_INI()
{
	// This will read your .ini file settings or create them if not available (including creating the file).

	GetPrivateProfileString("Settings", "ChatChannel", "NULL", ChatChannel, MAX_STRING, INIFileName);
	if (!_strnicmp(ChatChannel, "NULL", 4)) {
		WritePrivateProfileString("Settings", "ChatChannel", "/bc", INIFileName);
		sprintf_s(ChatChannel, "/bc");
	}
	GetPrivateProfileString("Settings", "ChannelMessage", "NULL", ChannelMessage, MAX_STRING, INIFileName);
	if (!_strnicmp(ChannelMessage, "NULL", 4)) {
		WritePrivateProfileString("Settings", "ChannelMessage", " sent ${Me} a tell that said: ", INIFileName);
		sprintf_s(ChannelMessage, " sent ${Me} a tell that said: ");
	}
	RelayTellsInGame = BoolFromINI("Settings", "RelayTellsInGame");
	RelayAFKMessage = BoolFromINI("Settings", "RelayAFKMessage");
	RelayTraderTells = BoolFromINI("Settings", "RelayTraderTells");
	RelayPetTells = BoolFromINI("Pet", "RelayPetTells");
	RelayPetAttack = BoolFromINI("Pet", "RelayPetAttack");
	RelayPetSuspend = BoolFromINI("Pet", "RelayPetSuspend");
	RelayPetMezzed = BoolFromINI("Pet", "RelayPetMezzed");
	RelayNPCTells = BoolFromINI("NPC", "RelayNPCTells");
	RelayToMail = BoolFromINI("Mail", "RelayToMail");
	GetPrivateProfileString("Mail", "EmailTo", "NULL", EmailTo, MAX_STRING, INIFileName);
	if (!_strnicmp(EmailTo, "NULL", 4)) {
		WritePrivateProfileString("Mail", "EmailTo", "WhoIsEmailGoingTo@Theiraddress.com", INIFileName);
		sprintf_s(EmailTo, "WhoIsEmailGoingTo@Theiraddress.com");
		WriteChatColor("ERROR:You must enter your own information and reload the ini if you wish to use mail relay", CONCOLOR_RED);
	}
	GetPrivateProfileString("Mail", "EmailFrom", "NULL", EmailFrom, MAX_STRING, INIFileName);
	if (!_strnicmp(EmailFrom, "NULL", 4)) {
		WritePrivateProfileString("Mail", "EmailFrom", "EQTellReceived@ingame.com", INIFileName);
		sprintf_s(EmailFrom, "EQTellReceived@ingame.com");
	}
	GetPrivateProfileString("Mail", "Subject", "NULL", Subject, MAX_STRING, INIFileName);
	if (!_strnicmp(Subject, "NULL", 4)) {
		WritePrivateProfileString("Mail", "Subject", "Tell Received", INIFileName);
		sprintf_s(Subject, "Tell Received");
	}
	GetPrivateProfileString("Mail", "Server", "NULL", Server, MAX_STRING, INIFileName);
	if (!_strnicmp(Server, "NULL", 4)) {
		WritePrivateProfileString("Mail", "Server", "smtp.yourserver.com", INIFileName);
		sprintf_s(Server, "smtp.yourserver.com");
		WriteChatColor("ERROR:You must enter your own information and reload the ini if you wish to use mail relay", CONCOLOR_RED);
	}
	GetPrivateProfileString("Mail", "UserName", "NULL", UserName, MAX_STRING, INIFileName);
	if (!_strnicmp(UserName, "NULL", 4)) {
		WritePrivateProfileString("Mail", "UserName", "YourUserName", INIFileName);
		sprintf_s(UserName, "YourUserName");
		WriteChatColor("ERROR:You must enter your own information and reload the ini if you wish to use mail relay", CONCOLOR_RED);
	}
	GetPrivateProfileString("Mail", "Password", "NULL", Password, MAX_STRING, INIFileName);
	if (!_strnicmp(Password, "NULL", 4)) {
		WritePrivateProfileString("Mail", "Password", "YourPassword", INIFileName);
		sprintf_s(Password, "YourPassword");
		WriteChatColor("ERROR:You must enter your own information and reload the ini if you wish to use mail relay", CONCOLOR_RED);
	}
	GetCustomTells();
}

void AddCustomTell(PSPAWNINFO pChar, PCHAR szLine) {
	char szTemp[MAX_STRING];

	TotalCustomTells++;
	sprintf_s(szTemp, "Tell%i", TotalCustomTells);
	WritePrivateProfileString("CustomTells", szTemp, szLine, INIFileName);

	Load_INI();

}

void Reload_INI(PSPAWNINFO pChar, PCHAR szLine) {
	WriteChatColor("Reloading the INI File", CONCOLOR_RED);
	Load_INI();
}
PLUGIN_API void SetGameState(DWORD GameState)
{
	if (GameState == GAMESTATE_INGAME) {
		if (pLocalPC) {
			sprintf_s(INIFileName, "%s\\MQ2Relaytells_%s_%s.ini", gPathConfig, pLocalPC->Name, GetServerShortName());
			Load_INI();
		}
	}
}
void RelayHelp(PSPAWNINFO pChar, PCHAR szLine) {

	// This will display the current ini file settings

	WriteChatf("\ay====================\ax");
	WriteChatf("%s::Version [\ag%1.4f\ax] Loaded!", mqplugin::PluginName, MQ2Version);
	WriteChatf("\ay====================\ax");
	WriteChatf("\ayCurrent settings: \ag(0=FALSE, 1=TRUE)\ax");
	WriteChatf("\ay====================\ax");
	WriteChatf("Relay Tells to Chat Channel: \ag%d\ax", RelayTellsInGame);
	WriteChatf("Chat Channel: \ag%s\ax", ChatChannel);
	WriteChatf("Relay AFK Messages: \ag%d\ax", RelayAFKMessage);
	WriteChatf("Relay Pet Messages: \ag%d\ax", RelayPetTells);
	WriteChatf("Relay Pet Attacking Messages: \ag%d\ax", RelayPetAttack);
	WriteChatf("Relay Pet Suspend Messages: \ag%d\ax", RelayPetSuspend);
	WriteChatf("Relay Pet Can't Wake Mezzed Messages: \ag%d\ax", RelayPetMezzed);
	WriteChatf("Relay NPC Tells: \ag%d\ax", RelayNPCTells);
	WriteChatf("Relay Trader Tells: \ag%d\ax", RelayTraderTells);
	WriteChatf("\ay====================\ax");
	WriteChatf("\ayType /relaymailhelp for mail settings\ax");
	WriteChatf("\ay====================\ax");

}
void RelayMailHelp(PSPAWNINFO pChar, PCHAR szLine) {

	// This will display the current ini file settings for Mail Relay

	WriteChatf("\ay====================\ax");
	WriteChatf("%s::Version [\ag%1.4f\ax] Loaded!", mqplugin::PluginName, MQ2Version);
	WriteChatf("\ay====================\ax");
	WriteChatf("\ayCurrent settings: \ag(0=FALSE, 1=TRUE)\ax");
	WriteChatf("\ay====================\ax");
	WriteChatf("Relay To Email: \ag%d\ax", RelayToMail);
	WriteChatf("Email To: \ag%s\ax", EmailTo);
	WriteChatf("Email From: \ag%s\ax", EmailFrom);
	WriteChatf("Subject: \ag%s\ax", Subject);
	WriteChatf("Server: \ag%s\ax", Server);
	WriteChatf("UserName: \ag%s\ax", UserName);
	WriteChatf("Password: \ag%s\ax", Password);
	WriteChatf("\ay====================\ax");
}

void RTSetKey(PSPAWNINFO pChar, PCHAR szLine)
{
	char szApp[MAX_STRING], szKey[MAX_STRING], szValue[MAX_STRING];
	GetArg(szApp, szLine, 1);
	GetArg(szKey, szLine, 2);
	GetArg(szValue, szLine, 3);
	WritePrivateProfileString(szApp, szKey, szValue, INIFileName);
	Load_INI();
	return;
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	// Checks for incoming chat

	if (strstr(Line, "tells you, ")) DoTellRelay(Line);
	if (strstr(Line, "told you, ")) DoTellRelay(Line);
	return 0;
}
