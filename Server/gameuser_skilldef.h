#pragma once

struct evtick;

class skill_interface
{
public:
	skill_interface();
	~skill_interface();
	virtual void tick(const evtick &arg_evt) = 0;

private:

};

class skill1_def : public skill_interface
{
public:
	skill1_def();
	~skill1_def();
	virtual void tick(const evtick &arg_evt);

private:

};

class skill2_def : public skill_interface
{
public:
	skill2_def();
	~skill2_def();
	virtual void tick(const evtick &arg_evt);

private:

};


