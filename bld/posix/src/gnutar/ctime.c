/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 * Original comment: "ctime.c   yah (yet another hack) from date.c"
 * Note - this came from the comp.os.minix newsgroup, but I
 * don't know who the author is; he got it from date.c, but somehow
 * made a mistake in the number of seconds per year -- he thought
 * there were 60 hours in a day.  So I fixed that, but other
 * than that it's all the same... -- Eric Roskos
 */


int days_per_month[] =
  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
char *months[] =
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
char *days[] =
  { "Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed" };

struct {
        int year, month, day, hour, min, sec;
} tm;

long s_p_min;
long s_p_hour;
long s_p_day;
long s_p_year;

char t_buf[26];

char *ctime(t)
long *t;
{
  long tt;

  s_p_min  = 60L;
  s_p_hour = 60L * 60L;
  s_p_day  = 60L * 60L * 24L;
  s_p_year = 60L * 60L * 24L * 365L;

  tt = *t;
  cv_time(tt);

  sprintf(t_buf,"%s %s %02d %02d:%02d:%02d %04d\n", days[(tt / s_p_day) % 7],
           months[tm.month], tm.day, tm.hour, tm.min, tm.sec, tm.year);
  return(t_buf);
}

cv_time(t)
long t;
{
  tm.year = 0;
  tm.month = 0;
  tm.day = 1;
  tm.hour = 0;
  tm.min = 0;
  tm.sec = 0;
  while (t >= s_p_year) {
        if (((tm.year + 2) % 4) == 0)
                t -= s_p_day;
        tm.year += 1;
        t -= s_p_year;
  }
  if (((tm.year + 2) % 4) == 0)
        days_per_month[1]++;
  tm.year += 1970;
  while ( t >= (days_per_month[tm.month] * s_p_day))
        t -= days_per_month[tm.month++] * s_p_day;
  while (t >= s_p_day) {
        t -= s_p_day;
        tm.day++;
  }
  while (t >= s_p_hour) {
        t -= s_p_hour;
        tm.hour++;
  }
  while (t >= s_p_min) {
        t -= s_p_min;
        tm.min++;
  }
  tm.sec = (int) t;
}
