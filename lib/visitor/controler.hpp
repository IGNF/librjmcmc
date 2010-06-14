#ifndef BUILDING_CONTROLER_HPP
#define BUILDING_CONTROLER_HPP

class Controler {
public:
	virtual void go() = 0;
	virtual void stop() = 0;
	virtual void param_visibility(bool) = 0;
	virtual void chart_visibility(bool) = 0;
	virtual bool param_visibility() const = 0;
	virtual bool chart_visibility() const = 0;
};

#endif
