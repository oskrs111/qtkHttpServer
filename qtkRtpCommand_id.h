#ifndef QTKRTPCOMMAND_ID__H
#define QTKRTPCOMMAND_ID__H
//OSLL: Define here the id's for your RPC commands as well it's text names, or 'alias'. 
namespace rtp_command_id
{
	enum 
	{
		rci_TestCommand = 100,
		rci_Last	
	};
}

struct rtpCommandStruct
{
	char* p_commandAlias;
	int m_commandId;
}

static struct rtpCommandStruct rtpCommands [] =
{
{"test", rci_TestCommand}
};

#endif 
