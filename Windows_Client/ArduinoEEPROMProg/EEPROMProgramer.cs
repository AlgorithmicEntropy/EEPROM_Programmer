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

            //welcome text

            Console.WriteLine(AsciiHolder.banner);

            Console.WriteLine("Current functions are: \n\tread\n\twrite");

            while (true)
            {
                string entry = Console.ReadLine();
                if (entry == "read")
                {
                    Console.WriteLine("Please enter start address");
                    uint startAddress = Convert.ToByte(Console.ReadLine(), 16);

                    Console.WriteLine("Please enter end address");
                    uint endAddress = Convert.ToByte(Console.ReadLine(), 16);

                    byte[] code = new byte[] { 0x02, (byte)(startAddress >> 8), (byte)startAddress, 0x06, (byte)(startAddress >> 8), (byte)endAddress, 0x04, 0x00, 0x00 };
                    con.Write(code);
                    continue;
                }

                else if (entry == "write")
                {
                    Console.WriteLine("Please enter start address");
                    uint startAddress = Convert.ToByte(Console.ReadLine(), 16);

                    Console.WriteLine("Please enter end address");
                    uint endAddress = Convert.ToByte(Console.ReadLine(), 16);

                    //get value
                    Console.WriteLine("Please enter value");
                    byte val = Convert.ToByte(Console.ReadLine(), 16);

                    byte[] code = new byte[] { 0x01, 0x00, 0x00 };
                    //send
                    con.Write(code);
                    for (uint i = startAddress; i < endAddress; i++)
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
                else
                {
                    Console.WriteLine("Unknown command");
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
