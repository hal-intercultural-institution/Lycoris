#include "game/load_screen.h"

void lycoris::game::load_screen::set_load_status(status status)
{
	status_ = status;
}

lycoris::game::load_screen::status lycoris::game::load_screen::get_load_status() const
{
	return status_;
}
