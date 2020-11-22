/**
 * /config/options.h
 *
 * Copyright (C) 2017 HinsShum
 *
 * platform.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#ifdef __cpluscplus
extern "c"
{
#endif /* __cpluscplus */

#if defined PLAT_0090016
    #include "config/0090016/options.h"
#elif defined PLAT_0190016
    #include "config/0190016/options.h"
#elif defined PLAT_0290016
    #include "config/0290016/options.h"
#elif defined PLAT_000B822
    #include "config/000B822/options.h"
#elif defined PLAT_RSTF103ZET6
    #include "config/RSTF103ZET6/options.h"
#endif

#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __OPTIONS_H__ */
