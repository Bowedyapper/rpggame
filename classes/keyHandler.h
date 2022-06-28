/*****************************************************************//**
 * \file   keyHandler.h
 * \brief  Will be the main handler for key input once I work out
 * 		   the best way to handle movement
 * 
 * \author Jason Hewitt <bowedyapper@live.co.uk>
 * \date   June 2022
 *********************************************************************/

namespace keys{
	void handleKeyDown(int keyCode);
	void handleKeyUp(int keyCode);
};

void keys::handleKeyDown(int keyCode) {
	std::cout << "DOWN: " << std::to_string(keyCode) << std::endl;
}

void keys::handleKeyUp(int keyCode){
	std::cout << "UP: " << std::to_string(keyCode) << std::endl;
}
