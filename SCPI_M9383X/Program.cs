using System;
using Ivi.Visa.Interop;
using System.Net.Sockets;
using System.Net;




namespace SCPI_M9383X
{
    class Program
    {
        static string GetQuery(Socket soc)
        {
            byte[] buffer = new byte[1024];
            int iRx = soc.Receive(buffer);
            char[] chars = new char[iRx];

            System.Text.Decoder d = System.Text.Encoding.UTF8.GetDecoder();
            int charLen = d.GetChars(buffer, 0, iRx, chars, 0);
            System.String recv = new System.String(chars);
            return recv;
        }

        static string SendQuery_R(Socket soc, string Command)
        {
            byte[] byData = System.Text.Encoding.ASCII.GetBytes(Command + "\r\n");
            soc.Send(byData);
            string Result = GetQuery(soc);
            byData = System.Text.Encoding.ASCII.GetBytes(":SYST:ERR?\r\n");
            soc.Send(byData);
            string Error = GetQuery(soc);
            Console.WriteLine("Command {0} : Response {1} - {2}", Command, Result, Error);
            return Result;
        }

        static void SendCommand(Socket soc, string Command)
        {
            byte[] byData = System.Text.Encoding.ASCII.GetBytes(Command + "\r\n");
            soc.Send(byData);
            byData = System.Text.Encoding.ASCII.GetBytes(":SYST:ERR?\r\n");
            soc.Send(byData);
            string Error = GetQuery(soc);
            Console.WriteLine("Command {0} - {1}", Command, Error);
        }

        static void Main(string[] args)
        {
            // start server (it's just a label to scpi server)
            //System.Diagnostics.Process.Start("server");

            string Error;
            string Result;

            QueryPerfCounter PerfCounter = new QueryPerfCounter();
            

            Socket soc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            System.Net.IPAddress ipAdd = System.Net.IPAddress.Parse("192.168.0.1");
            System.Net.IPEndPoint remoteEP = new IPEndPoint(ipAdd, 5025);
            soc.Connect(remoteEP);

            PerfCounter.Start();

            SendCommand(soc, "*RST");
            SendQuery_R(soc, "*IDN?");

            PerfCounter.Stop();
            PerfCounter.Duration(1);
            Console.WriteLine(PerfCounter.Duration(1));

            Console.ReadLine();


        }


    }
}

