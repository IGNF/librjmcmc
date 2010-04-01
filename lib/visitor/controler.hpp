#ifndef BUILDING_CONTROLER_HPP
#define BUILDING_CONTROLER_HPP

template<typename View>
class Controler {
public:
	virtual void go(const View &view) {}
	virtual void stop() {}
	virtual void toggle_param_visibility() {}
	virtual void toggle_chart_visibility() {}
};

#endif
