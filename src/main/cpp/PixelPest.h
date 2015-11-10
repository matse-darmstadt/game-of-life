#pragma once

//möglichst wenig in Headerfiles einbinden!!

class PixelPest
{
private:
	bool running;
public:
	PixelPest(void);
	~PixelPest(void);

	void run();
	void stop();
};

