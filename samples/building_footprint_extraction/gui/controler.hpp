#ifndef BUILDING_CONTROLER_HPP
#define BUILDING_CONTROLER_HPP

class Controler {
public:
	virtual void go(void *image) {}
	virtual void stop() {}
	virtual void toggle_param_visibility() {}
	virtual void toggle_chart_visibility() {}
};

#endif
