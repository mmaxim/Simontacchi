//Mike Maxim
//Main engine driver code

#include <engine.h>

Engine* Engine::m_instance = NULL;

Engine::Engine() {
    
}

Engine* Engine::getInstance() {
    if (m_instance == NULL) {
        m_instance = new Engine();
    }
    
    return m_instance;
}

void Engine::init() {

    ConfigFile* cfg = ConfigFile::getInstance();
    PlayerInit pinit;

    WRITEPIPE("<Simontacchi Chess Engine by Mike Maxim>\n\n");
    WRITEPIPE("*** Initializing Engine\n");

    m_env = ENGINE_ENV_CONSOLE;

    //Load the configuration from the configuration file
    if (!cfg->loadFile(ENGINE_CONFIG_FILE)) {
        WRITEPIPE("ENGINE: Error: Invalid Configuration File: ABORTING\n");
        exit(0);
    }

    //Init the static data
    Board::static_initialization();

    //Load in the controller
    if (cfg->getValue("CONTROLLER") == "winboard")
        m_controller = Controller::getInstance(CONTROLLER_WINBOARD);
    else if (cfg->getValue("CONTROLLER") == "uci")
        m_controller = Controller::getInstance(CONTROLLER_UCI);

    //Init the player original options
    pinit.m_bookfile = cfg->getValue("BOOK"); 
    pinit.m_nulldepth = atoi(cfg->getValue("NULLDEPTH").c_str()); 
    pinit.m_showthinking = ("true" == cfg->getValue("SHOWTHINKING"));
    pinit.m_transenabled = ("true" == cfg->getValue("TRANSENABLED"));
    pinit.m_transsize = (atoi(cfg->getValue("TRANSSIZE").c_str())*1024*1024)/sizeof(TTEntry);
    pinit.m_usebook = ("true" == cfg->getValue("USEBOOK"));
    pinit.m_usetiming = ("true" == cfg->getValue("USETIMING"));
    pinit.m_idstart = atoi(cfg->getValue("IDSTART").c_str());
    pinit.m_contempt = atoi(cfg->getValue("CONTEMPT").c_str());
    pinit.m_pvs = ("true" == cfg->getValue("PVSENABLED"));
    pinit.m_ptranssize = (atoi(cfg->getValue("PTRANSSIZE").c_str())*1024*1024)/sizeof(PawnHashEntry);
    pinit.m_evalcachesize = (atoi(cfg->getValue("EVALCACHESIZE").c_str())*1024*1024)/sizeof(EvalCacheEntry);
    pinit.m_lazy = ("true" == cfg->getValue("LAZYEVAL"));
    pinit.m_lazywindow = atoi(cfg->getValue("LAZYWINDOW").c_str());
    pinit.m_asp = ("true" == cfg->getValue("ASPIRATION"));
    pinit.m_abwindow = atoi(cfg->getValue("ASPWINDOW").c_str());
    pinit.m_extfutile = atoi(cfg->getValue("EXTFUTILE").c_str());
    pinit.m_razor = atoi(cfg->getValue("RAZOR").c_str());
    pinit.m_nullenabled = ("true" == cfg->getValue("NULLENABLED"));

    if (cfg->getValue("DISPLAY") == "crafty")
        pinit.m_display = DISPLAY_MANAGER_CRAFTY;
    else if (cfg->getValue("DISPLAY") == "legacy")
        pinit.m_display = DISPLAY_MANAGER_LEGACY;
    else
        pinit.m_display = DISPLAY_MANAGER_LEGACY;

    (Controller::getInstance(CONTROLLER_WINBOARD))->getPlayer()->init(pinit);
    (Controller::getInstance(CONTROLLER_UCI))->getPlayer()->init(pinit);

    //Info
    WRITEPIPE(string("*** Depth-Replace Transposition Table: (Entries: " + itos(pinit.m_transsize) + ") "
        + "(Size: " + dtos(double(sizeof(TTEntry)*pinit.m_transsize)/(1024.0*1024.0)) + "MB)\n").c_str());
    WRITEPIPE(string("*** Full Evaluation Cache: (Entries: " + itos(pinit.m_evalcachesize) + ") (Size: " +
        dtos(double((sizeof(EvalCacheEntry)*pinit.m_evalcachesize))/(1024.0*1024.0))
         + "MB)\n").c_str());
    WRITEPIPE(string("*** Pawn Evaluation Cache: (Entries: " + itos(pinit.m_ptranssize) + ") (Size: " +
        dtos(double((sizeof(PawnHashEntry)*pinit.m_ptranssize))/(1024.0*1024.0))
         + "MB)\n").c_str());
    WRITEPIPE(string("*** Opening Book Positions: " + itos(m_controller->getPlayer()->get_book()->get_positions()) + "\n\n").c_str());
    WRITEPIPE("*** Enter command \"help\" for instructions on usage\n");
    WRITEPIPE("*** Usage under XBoard/WinBoard is highly recommended\n\n");

    m_controller->init(cfg);
}

int Engine::main() {
    
    char command[163840];
    string strcmd,str,str2;
    int bytesread=0,i;

    //Enter into read loop
    while (true) {
//      _read(0,command+bytesread,1);
        READPIPE(command+bytesread,1);
        if (command[bytesread] == '\n' || command[bytesread] == '\0') {
            command[bytesread]='\0';
            strcmd = string(command);
            
            if (strcmd == "help") {
                output_help();
                m_controller->prompt();
            }
            else if (strcmd == "version") {
                WRITEPIPE(string(version_str() + "\n").c_str());
                m_controller->prompt();
            }
            else if (strcmd == "uci") {
                m_controller = Controller::getInstance(CONTROLLER_UCI);
                m_controller->init(ConfigFile::getInstance());
                m_controller->process_command(strcmd);
            }   
            else if (strcmd.substr(0,10) == "createbook") {
                StringTokenizer tok(command, " ");

                if (tok.size() == 1) {
                    bytesread = 0;
                    WRITEPIPE("ERROR: Not enough parameters for createbook\n");
                    m_controller->prompt();
                    continue;
                }
                str = tok[1]; 

                if (tok.size() == 2) {
                    bytesread = 0;
                    WRITEPIPE("ERROR: Not enough parameters for createbook\n");
                    m_controller->prompt();
                    continue;
                }
                str2 = tok[2]; 
                
                if (tok.size() == 3) {
                    bytesread = 0;
                    WRITEPIPE("ERROR: Not enough parameters for createbook\n");
                    m_controller->prompt();
                    continue;
                }
                i = atoi(tok[3].c_str());
                createbook(str,str2,i);

                m_controller->prompt();
            }
            else {
                try {           
                    m_controller->process_command(strcmd);
                } catch (Exception e) {
                    WRITEPIPE(string(e.getMessage() + "\n").c_str());
                    m_controller->prompt();
                }
            }
        
            bytesread = 0;
            continue;
        }
        bytesread++;
    }

    return 125;
}

void Engine::createbook(const string& file, const string& ofile, int depth) {

    BookGenerator bookgen;
    bookgen.create_book(file,ofile,depth);

}

string Engine::version_str() {

    string vstr="",osstr,cpustr="",memstr="";
  
    vstr += string(NAMESTR) + " v" + string(CVERSION) + " by Mike Maxim\n";

#ifdef WIN32
  OSVERSIONINFO os;
    MEMORYSTATUS mem;   

  os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os);
  switch (os.dwMinorVersion) {
    case 0:
        if (os.dwMajorVersion == 4)
            osstr = "Windows NT 4.0";
        else if (os.dwMajorVersion == 5)
            osstr = "Windows 2000";
        break;
    case 10:
        osstr = "Windows 98";
        break;
    case 90:
        osstr = "Windows Me";
        break;
    case 51:
        osstr = "Windows NT 3.51";
        break; 
    case 1:
        osstr = "Windows XP";
        break;
    case 2:
        osstr = "Windows 2003 Server";
    }

    if (IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE))
        cpustr += "MMX: Yes";
    else
        cpustr += "MMX: No";

    if (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
        cpustr += " SSE2: Yes";
    else
        cpustr += " SSE2: No";

    GlobalMemoryStatus(&mem);
    memstr += itos((int)mem.dwTotalPhys/(1000*1000)) + "MB Total";
    memstr += " " + itos((int)mem.dwAvailPhys/(1000*1000)) + "MB Avail";

    vstr += "OS: [" + osstr + "]" + " CPU: [" + cpustr + "]" + " MEM: [" + memstr + "]";
#endif

    return vstr;
}

void Engine::output_help() {

    cout << endl << "Simontacchi Help" << endl << "------------------" << endl << endl;
    cout << "new: Reset the game" << endl;
    cout << "<SAN>: Make a move" << endl;
    cout << "clocks: Display time remaining" << endl;
    cout << "level: Set the game time (see Winboard docs for usage)" << endl;
    cout << "go: Force Simontacchi to move in the current position" << endl;
    cout << "analyze: Let Simontacchi assess the current position until explicitly stopped" << endl;
    cout << "force: Put Simontacchi into force mode, enabling the user to move pieces without the engine making a move" << endl;
    cout << "playother: Leave force mode and set the engine to play the opposite color" << endl;
    cout << "exit: Leave analysis mode" << endl;
    cout << ".: Get current info about Simontacchi's thinking in analysis mode" << endl;
    cout << "hint: Ask Simontacchi for a hint" << endl;
    cout << "bk: Get Simontacchi's book knowledge on current position" << endl;
    cout << "bkoff: Turn the book off" << endl;
    cout << "bkon: Turn the book on (default)" << endl;
    cout << "hard: Turn pondering on (Simontacchi will think during opponent move)" << endl;
    cout << "easy: Turn pondering off" << endl;
    cout << "setboard <FEN>: Set the current position with an FEN formatted string" << endl;
    cout << "tofen: Get the FEN string for the current position" << endl;
    cout << "post: Turn Simontacchi verbose thinking on" << endl;
    cout << "nopost: Turn off the verbose thinking" << endl;
    cout << "show: Display the current position" << endl;
    cout << "eval: Get Simontacchi's quick take on the position" << endl;
    cout << "sd <INT>: Set the maximum search depth Simontacchi should look" << endl;
    cout << "genmoves: List the legal moves for the current position" << endl;
    cout << "load <FILE>: Load a PGN file" << endl;
    cout << "playloaded <INDEX>: Play back the INDEX game from the most recently loaded PGN file" << endl;
    cout << "createbook <PGN file> <book file> <ply>: Create a book from the specified file to the specified depth" << endl;
    cout << "version: Display engine version" << endl;
    cout << "quit: Exit Simontacchi" << endl << endl;
}
