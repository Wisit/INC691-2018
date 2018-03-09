using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace EccWebCompressor
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("ecc-web-compressor version 0.0");


            string inputFile = @"C:\Users\santi\Desktop\INC691-2018\Embedded-Development-Framework\library\addon\esp8266\page\index.html";
            string outputFile = @"C:\Users\santi\Desktop\INC691-2018\Embedded-Development-Framework\library\addon\esp8266\page\index.out.html"; ;

            int count1=0, count2=0;


            //---------------------------------------------------------------------------
            //!! Read input file
            string lines = "";
            try
            {   // Open the text file using a stream reader.
                using (StreamReader sr = new StreamReader(inputFile))
                {
                    // Read the stream to a string, and write the string to the console.
                    lines = sr.ReadToEnd();
                    count1 = lines.Length;
                    Console.WriteLine(lines);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("The file could not be read:");
                Console.WriteLine(e.Message);
            }


            //---------------------------------------------------------------------------
            //!! Processing
            //!!    1) Remove new line
            //lines = lines.Replace("\r\n", "");
            //lines = Regex.Replace(lines, @"\t|\n|\r", "", RegexOptions.Multiline);

            //!!    2) Remove multiple spaces
            //lines = Regex.Replace(lines, @"\s+", " ");
            lines = Regex.Replace(lines, @"\s+", " ", RegexOptions.Multiline);


            //!!    3) > <
            while (lines.Contains("> <")) lines = lines.Replace("> <", "><");

            //!!    4) =[SPACE]
            while (lines.Contains("= ")) lines = lines.Replace("= ", "=");

            //!!    4) [SPACE]=
            while (lines.Contains(" =")) lines = lines.Replace(" =", "=");

            //!!    5) '[SPACE]
            while (lines.Contains("' ")) lines = lines.Replace("' ", "'");

            //!!    6) [SPACE]'
            while (lines.Contains(" '")) lines = lines.Replace(" '", "'");

            //!!    6) [SPACE]'
            while (lines.Contains("\"")) lines = lines.Replace("\"", "'");


            count2 = lines.Length;

            

            Console.WriteLine(count1.ToString() + ", " + count2.ToString());

            lines = "const char * homeHtml = " + "\"" + lines + "\";";
            Console.WriteLine(lines);


            using (System.IO.StreamWriter file =
            new System.IO.StreamWriter(outputFile, false))
            {
                file.WriteLine(lines);
            }




            Console.ReadKey();
        }
    }
}
