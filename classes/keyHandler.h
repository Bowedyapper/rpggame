namespace keys{
	void handleKeyDown(int thing);
	void handleKeyUp(int thing);
};

void keys::handleKeyDown(int thing) {
	std::cout << "DOWN: " << std::to_string(thing) << std::endl;
}

void keys::handleKeyUp(int thing){
	std::cout << "UP: " << std::to_string(thing) << std::endl;
}
