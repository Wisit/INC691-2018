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


            string pageRoot = @"C:\Users\santi\Desktop\INC691-2018\Embedded-Development-Framework\dev\dev10_response_header\webpage\";
            string indexHtml = "index";
            string indexCss = "index";
            string indexJs = "index";

            List<string> fileInList = new List<string>();
            fileInList.Add(pageRoot + indexHtml + ".html");
            fileInList.Add(pageRoot + indexCss + ".css");
            fileInList.Add(pageRoot + indexJs + ".js");

            List<string> fileOutList = new List<string>();
            fileOutList.Add(pageRoot + indexHtml + ".out.html");
            fileOutList.Add(pageRoot + indexCss + ".out.css");
            fileOutList.Add(pageRoot + indexJs + ".out.js");

            List<string> varNameList = new List<string>();
            varNameList.Add(indexHtml + "Html");
            varNameList.Add(indexCss + "Css");
            varNameList.Add(indexJs + "Js");


            for (int i = 0; i < fileInList.Count; i++)
            {

                string inputFile = fileInList[i];   // @"C:\Users\santi\Desktop\INC691-2018\Embedded-Development-Framework\dev\dev07_default_page\webpage\index.html";
                string outputFile = fileOutList[i]; // @"C:\Users\santi\Desktop\INC691-2018\Embedded-Development-Framework\dev\dev07_default_page\webpage\index.out.html"; ;

                int count1 = 0, count2 = 0;


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
                        //Console.WriteLine(lines);
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

                //!!    7) >[SPACE]
                while (lines.Contains("> ")) lines = lines.Replace("> ", ">");

                //!!    8) [SPACE]<
                while (lines.Contains(" <")) lines = lines.Replace(" <", "<");


                //!! CSS


                //!!    9) {[SPACE]
                while (lines.Contains("{ ")) lines = lines.Replace("{ ", "{");

                //!!    10) }[SPACE]
                while (lines.Contains("} ")) lines = lines.Replace("} ", "}");

                //!!    11) [SPACE]{
                while (lines.Contains(" {")) lines = lines.Replace(" {", "{");

                //!!    12) [SPACE]}
                while (lines.Contains(" }")) lines = lines.Replace(" }", "}");


                //!!    13) [SPACE]:
                while (lines.Contains(" :")) lines = lines.Replace(" :", ":");

                //!!    14) :[SPACE]
                while (lines.Contains(": ")) lines = lines.Replace(": ", ":");

                //!!    15) [SPACE]:
                while (lines.Contains(" ;")) lines = lines.Replace(" ;", ";");

                //!!    16) :[SPACE]
                while (lines.Contains("; ")) lines = lines.Replace("; ", ";");


                //!! JS

                //!!    17) [SPACE],
                while (lines.Contains(" ,")) lines = lines.Replace(" ,", ",");

                //!!    18) ,[SPACE]
                while (lines.Contains(", ")) lines = lines.Replace(", ", ",");

                //!!    19) [SPACE]+
                while (lines.Contains(" +")) lines = lines.Replace(" +", "+");

                //!!    20) ,[SPACE]
                while (lines.Contains("+ ")) lines = lines.Replace("+ ", "+");

                //!!    21) [SPACE]-
                while (lines.Contains(" -")) lines = lines.Replace(" -", "-");

                //!!    22) -[SPACE]
                while (lines.Contains("- ")) lines = lines.Replace("- ", "-");

                //!!    23) [SPACE]*
                while (lines.Contains(" *")) lines = lines.Replace(" *", "*");

                //!!    24) *[SPACE]
                while (lines.Contains("* ")) lines = lines.Replace("* ", "*");

                //!!    25) [SPACE]/
                while (lines.Contains(" /")) lines = lines.Replace(" /", "/");

                //!!    26) *[SPACE]
                while (lines.Contains("/ ")) lines = lines.Replace("/ ", "/");




                count2 = lines.Length;



                Console.WriteLine(count1.ToString() + ", " + count2.ToString());

                lines = "const char * " + varNameList[i] + "=" + "\"" + lines + "\";";
                Console.WriteLine(lines);


                using (System.IO.StreamWriter file = new System.IO.StreamWriter(outputFile, false))
                {
                    file.WriteLine(lines);
                }
            }// for
            Console.ReadKey();
        }// Main
    }
}


// HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nServer: ECC-Lab\r\nConnection: close\r\n\r\n
// HTTP/1.1 200 OK\r\nContent-Type: application/x-javascript\r\nServer: ECC-Lab\r\nConnection: close\r\n\r\n