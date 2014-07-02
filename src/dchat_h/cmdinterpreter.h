/*
 *  Copyright (c) 2014 Christoph Mahrl
 *
 *  This file is part of DChat.
 *
 *  DChat is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DChat is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DChat.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef CMDINTERPRETER_H
#define CMDINTERPRETER_H

#include "types.h"


//*********************************
//     MAIN PARSING FUNCTION
//*********************************
int parse_cmd(dchat_conf_t* cnf, char* buf);


//*********************************
//       COMMAND FUNCTIONS
//*********************************
int parse_cmd_connect(dchat_conf_t* cnf, char* cmd);
void parse_cmd_help(void);
void parse_cmd_list(dchat_conf_t* cnf);

#endif
