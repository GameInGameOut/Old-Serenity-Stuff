using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace Interface
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            if (!Properties.Settings.Default.FirstLaunch || File.Exists("C:\\details.enc") || File.Exists("C:\\Program Files\\Project Serenity\\details.enc"))
                Application.Run(new Forms.Loader());

            else
                Application.Run(new Forms.Register());
        }
    }
}
