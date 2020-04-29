using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

using ArduinoEEPROMProg.Core;
using System.IO;

namespace ArduinoEEPROMProg
{
    class EEPROMProgramer
    {
        [STAThread]
        static void Main(string[] args)
        {
            //get port
            Console.WriteLine("Current serial ports:");

            foreach (var p in SerialPort.GetPortNames())
            {
                Console.WriteLine($"\t{p}");
            }

            SerialConnection con = null;

            while (con == null)
            {
                try
                {
                    string port = Console.ReadLine();
                    con = new SerialConnection(port.ToUpper());
                }
                catch (System.IO.IOException)
                {
                    Console.WriteLine("Invalid ports");
                } 
            }

            //clear
            Console.Clear();

            while (true)
            {
                string entry = Console.ReadLine();
                if (entry == "read")
                {
                    byte[] code = new byte[] { 0x02, 0x00, 0x00, 0x06, 0x00, 0xFF, 0x04, 0x00, 0x00 };
                    con.Write(code);
                    continue;
                }

                else if (entry == "write")
                {
                    //get value
                    byte val = Convert.ToByte(Console.ReadLine(), 16);
                    byte[] code = new byte[] { 0x01, 0x00, 0x00 };
                    //send
                    con.Write(code);
                    for (ushort i = 0; i <= 0x00FF; i++)
                    {
                        byte addH = (byte)(i >> 8);
                        byte addL = (byte)i;
                        Console.WriteLine($"write: {addH:X2}{addL:X2}\t{val:X2} ");
                        con.Write(new byte[] { addH, addL, val}); 
                    }
                    //end
                    con.Write(new byte[] { 0xFF, 0x00, 0x00 });
                    continue;
                }

                try
                {
                    byte data = Convert.ToByte(entry, 16);
                    con.Write(new byte[] { data });
                }
                catch (Exception)
                {
                    continue;
                }
            }
        }

    }
}
