#pragma once
#include "sfml_Application.h"

class sfml_cleansingFireApplication : public sfml_Application {
public:
	sfml_cleansingFireApplication();
	~sfml_cleansingFireApplication();

protected:
	virtual bool initSpecial() override;
	void drawSpecial() override;
	virtual GamePtr createGame(const GameInitParams& params) override;

	void refreshPauseColor();

	virtual bool onMouseMoved(const int x, const int y) override;
	virtual bool onMousePressed(const int x, const int y) override;

private:
	SpritePtr _restartBtnImg;
	SpritePtr _pauseBtnImg;
	SpritePtr _playBtnImg;
};
