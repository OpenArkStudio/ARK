using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Conv2UTF8
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("usage: Conv2UTF8 filepath");
                Console.WriteLine("File extension: *.h *.hpp *.c *.cpp *.cxx *.cc");
                return;
            }

            string path = args[0];
            if(!Directory.Exists(path))
            {
                Console.WriteLine("Invalid file path = " + path);
                return;
            }

            string[] all_files = Directory.GetFiles(path, "*.*", SearchOption.AllDirectories);
            foreach (string file in all_files)
            {
                String extension = Path.GetExtension(file);
                if (extension == ".h"|| 
                    extension == ".hpp" || 
                    extension == ".c" || 
                    extension == ".cpp" || 
                    extension == ".cxx" || 
                    extension == ".cc")
                {
                    var buffer = File.ReadAllBytes(file);
                    buffer = Encoding.Convert(Encoding.Default, Encoding.UTF8, buffer);
                    File.WriteAllBytes(file, buffer);
                }
            }

            Console.WriteLine("Convent all files to UTF8");
            Console.WriteLine("请按任意键继续...");
            Console.ReadKey();
        }
    }
}
