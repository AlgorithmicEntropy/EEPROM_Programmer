using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;
using System.IO;

namespace ArduinoEEPROMProg.Core
{
    public class SerialConnection : IDisposable
    {
        //const
        private const byte EscapeByte = 0x0F;
        private const byte AcknoladgeByte = 0x55;
        private const byte ErrorByte = 0x12;

        private byte lastByte = 0;

        private readonly SerialPort serial;
        private readonly Thread readThread;

        public SerialConnection(string port)
        {
            serial = new SerialPort
            {
                PortName = port,
                BaudRate = 9600,
                Parity = Parity.None,
                Handshake = Handshake.XOnXOff,
                ReadTimeout = 500,
                WriteTimeout = 500
            };

            serial.Open();
            serial.DiscardInBuffer();
            serial.DiscardOutBuffer();

            readThread = new Thread(Read);
            readThread.Start();
        }

        public void Dispose()
        {
            serial.Dispose();
        }

        public void Write(byte[] data)
        {
            foreach (var b in data)
            {
                serial.Write(new byte[] { b }, 0, 1);
            }
        }

        private void Read()
        {
            while (true)
            {
                int count = serial.BytesToRead;
                if (count != 0)
                {
                    for (int i = 0; i < count; i++)
                    {
                        var b = (byte)serial.ReadByte();
                        switch(b)
                        {
                            case AcknoladgeByte:
                                Console.WriteLine("ACK");
                                break;

                            case ErrorByte:
                                Console.WriteLine("ERR");
                                break;
                            case EscapeByte:
                                lastByte = b;
                                break;

                            default:
                                if (lastByte == EscapeByte)
                                {
                                    string s = ((byte)(~b)).ToString("X2");
                                    Console.WriteLine(s);
                                }
                                else
                                {
                                    Console.WriteLine(b.ToString("X2"));
                                }
                                lastByte = b;
                                break;
                        }

                    }
                }

                Thread.Sleep(10);
            }
        }

    }
}
