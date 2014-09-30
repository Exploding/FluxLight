using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FluxLightUI
{
    class Interval
    {
        public int[] startColor;

        public int[] endColor;

        public DateTime start;

        public DateTime end;

        protected double[] interval;

        public Interval(DateTime from, DateTime to, int[] sc, int[] ec)
        {
            start = new DateTime(2000, 1, 1, from.Hour, from.Minute, from.Second);

            end = new DateTime(2000, 1, 1, to.Hour, to.Minute, to.Second);

            startColor = sc;

            endColor = ec;

            //interval = getColorInterval(startColor, endColor);
        }

        public int[] getColor()
        {
            double minutesFromStart = DateTime.Now.TimeOfDay.Subtract(start.TimeOfDay).TotalMinutes;

            return new int[] { 
                (int)Math.Abs(startColor[0] - (minutesFromStart * interval[0])), 
                (int)Math.Abs(startColor[1] - (minutesFromStart * interval[1])), 
                (int)Math.Abs(startColor[2] - (minutesFromStart * interval[2])) 
            };
        }

        public double[] getColorInterval(int[] s, int[] e)
        {
            double minutes = end.TimeOfDay.Subtract(start.TimeOfDay).TotalMinutes;

            double r = (s[0] - e[0]) / minutes;
            double g = (s[1] - e[1]) / minutes;
            double b = (s[2] - e[2]) / minutes;

            return new double[] { r, g, b };
        }

        public DateTime getStart()
        {
            return start;
        }

        public DateTime getEnd()
        {
            return end;
        }
    }
}
