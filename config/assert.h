/**
 * @file /config/assert.h
 *
 * Copyright (C) 2020
 *
 * assert.h is free software: you can redistribute it and/or modify
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
 *
 * @author HinsShum hinsshum@qq.com
 */
#ifndef __CONFIG_ASSRET_H
#define __CONFIG_ASSRET_H

/*---------- includes ----------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "config/os.h"

/*---------- macro ----------*/
#undef assert
#ifdef CONFIG_DEBUG
#ifdef configASSERT
#define assert(x)           configASSERT(x)
#else
#define assert(x)           do { if(!(x)) { for(;;) }} while(0)
#endif
#else
#define assert(x)
#endif

/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
#endif /* __CONFIG_ASSRET_H */
