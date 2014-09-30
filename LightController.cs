using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace FluxLightUI
{
    class LightController
    {
        Interval[] intervals;

        SerialPort port;

        bool paired;

        int size = 0;

        struct Commands {
            static public byte SETCOLOR = 0;
            static public byte INTERVALON = 1;
            static public byte INTERVALOFF = 2;
            static public byte MUSICON = 3;
            static public byte MUSICOFF = 4;
            static public byte SETINTERVALS = 5;
            static public byte SETINTERVALTIMES = 6;
            static public byte PRINTINTERVALS = 7;
            static public byte PRINTINTERVALTIMES = 8;
            static public byte SETTIME = 9;
            static public byte PRINTTIME = 10;
            static public byte CLEARINTERVALS = 11;
            static public byte HANDSHAKE = 255;
        };

        Commands commands;

        public LightController()
        {
            commands = new Commands();

            paired = false;

            intervals = new Interval[10];

            if(handshake())
            {
                port.DataReceived += new SerialDataReceivedEventHandler(DataReceived);
            }
        }

        private bool handshake()
        {
            foreach (String portName in SerialPort.GetPortNames())
            {
                try
                {
                    port = new SerialPort(portName, 9600);

                    port.Open();

                    write(new byte[1] { Commands.HANDSHAKE }, 1);

                    if (port.BytesToRead > 0)
                    {
                        paired = true;

                        return true;
                    }

                }
                catch (Exception e) { }
            }

            return false;
        }

        private void DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            Console.WriteLine(port.ReadExisting());
        }

        public void addInterval(Interval interval)
        {
            if (size < 10)
            {
                intervals[size] = interval;
                size++;
            }
        }

        public void clearIntervals()
        {
            intervals = new Interval[] { };

            size = 0;

            write(new byte[1] { Commands.CLEARINTERVALS }, 1);
        }

        public void setTime()
        {
            DateTime now = DateTime.Now;

            byte[] buffer = new byte[3] {
                Commands.SETTIME,
                Convert.ToByte(now.Hour),
                Convert.ToByte(now.Minute)
            };

            write(buffer, 3);
        }

        public void getTime()
        {
            write(new byte[] { Commands.PRINTTIME }, 1);
        }

        public void setColor(int r, int g, int b)
        {
            byte[] buffer = new byte[4] {
                Commands.SETCOLOR,
                Convert.ToByte(r),
                Convert.ToByte(g),
                Convert.ToByte(b)
            };

            write(buffer, 4);
        }

        public void intervalModeOn()
        {
            write(new byte[1] { Commands.INTERVALON }, 1);
        }

        public void intervalModeOff()
        {
            write(new byte[1] { Commands.INTERVALOFF }, 1);
        }

        public void getIntervalTimes()
        {
            write(new byte[1] { Commands.PRINTINTERVALTIMES },  1);
        }

        public void getIntervals()
        {
            write(new byte[1] { Commands.PRINTINTERVALS }, 1);
        }

        public void setIntervals()
        {
            byte[] buffer = new byte[61];
            byte[] buffer2 = new byte[41];

            buffer[0] = Commands.SETINTERVALS;
            buffer2[0] = Commands.SETINTERVALTIMES;

            for (int i = 0; i < 10; i++)
            {
                for (int j = i * 6; j <= (i * 6); j+=6)
                {
                    if (intervals[i] != null)
                    {
                        buffer[j + 1] = Convert.ToByte(intervals[i].startColor[0]);
                        buffer[j + 2] = Convert.ToByte(intervals[i].startColor[1]);
                        buffer[j + 3] = Convert.ToByte(intervals[i].startColor[2]);
                        buffer[j + 4] = Convert.ToByte(intervals[i].endColor[0]);
                        buffer[j + 5] = Convert.ToByte(intervals[i].endColor[1]);
                        buffer[j + 6] = Convert.ToByte(intervals[i].endColor[2]);
                    }
                }

                for (int j = (i * 4); j <= (i*4); j+=4)
                {
                    if (intervals[i] != null)
                    {
                        buffer2[j + 1] = Convert.ToByte(intervals[i].start.Hour);
                        buffer2[j + 2] = Convert.ToByte(intervals[i].start.Minute);
                        buffer2[j + 3] = Convert.ToByte(intervals[i].end.Hour);
                        buffer2[j + 4] = Convert.ToByte(intervals[i].end.Minute);
                    }
                }
            }

            write(buffer, 61);
            write(buffer2, 41);
        }

        private void write(byte[] buffer, int size)
        {
            if(paired)
            {
                port.Write(buffer, 0, size);
            }
        }
    }
}
