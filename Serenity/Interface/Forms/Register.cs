using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Management;
using System.Security;
using System.Net;
using System.IO;

namespace Interface.Forms
{
    public partial class Register : Form
    {
        [DllImport("Kernel32.dll", SetLastError = true)]
        extern static bool GetVolumeInformation(string vol, StringBuilder name, int nameSize, out uint serialNum, out uint maxNameLen, out uint flags, StringBuilder fileSysName, int fileSysNameSize);

        public Register()
        {
            InitializeComponent();
        }

        private uint GetHwid1(string strDriveLetter)
        {
            uint serialNum, maxNameLen, flags;
            bool ok = GetVolumeInformation(strDriveLetter, null, 0, out serialNum, out maxNameLen, out flags, null, 0);
            return serialNum;
        }

        private string GetHwid2()
        {
            var mbs = new ManagementObjectSearcher("Select ProcessorId From Win32_processor");
            ManagementObjectCollection mbsList = mbs.Get();

            string id = "";
            foreach (ManagementObject mo in mbsList)
            {
                id = mo["ProcessorId"].ToString();
                return id;
            }

            return "";
        }

        private void butRegister_Click(object sender, EventArgs e)
        {
            //try
            //{
            //    StreamReader srHwid1 = new StreamReader("C:\\details.enc");
            //    StreamReader srHwid2 = new StreamReader("C:\\Program Files\\Project Serenity\\details.enc");
            //}

            //catch (Exception)
            //{
            //    MessageBox.Show("Error! You don't appear to be registered. This shouldn't happen. Tell laxai to get good at coding");
            //    return;
            //}

            if (File.Exists("Serenity.log"))
            {
                File.Delete("Serenity.log");
            }

            SecureString path1 = new NetworkCredential("", "C:\\details.enc").SecurePassword;
            SecureString path2 = new NetworkCredential("", "C:\\Program Files\\Project Serenity\\details.enc").SecurePassword;

            StreamWriter log = new StreamWriter("Serenity.log");

            log.WriteLine("Getting HWID");

            string hwid1 = GetHwid1("C:\\").ToString();
            string hwid2 = GetHwid2();
            string hwid = hwid1 + hwid2;
            hwid = hwid.ToLower();

            log.WriteLine("Got HWID");

            SecureString password = new NetworkCredential("", "Hacks69").SecurePassword;
            SecureString username = new NetworkCredential("", "b6_22246603").SecurePassword;
            SecureString server = new NetworkCredential("", "ftp://ftp.byethost6.com").SecurePassword;

            string test = new NetworkCredential("", server).Password;

            log.WriteLine("Connecting to ftp");

            Misc.Ftp ftp = new Misc.Ftp(new NetworkCredential("", server).Password, new NetworkCredential("", username).Password, new NetworkCredential("", password).Password);

            log.WriteLine("Connected");

            string[] dirs = ftp.directoryListSimple("htdocs/");

            log.WriteLine("Searching for user");

            foreach (string dir in dirs)
            {
                if (dir == Misc.Enc.Encrypt(txtUsername.Text))
                {
                    log.WriteLine("Already Registered");
                    MessageBox.Show("Username in use. Contact laxai if the account is yours.");
                    return;
                }
            }

            log.WriteLine("Username not in use");

            Random rand = new Random();

            string randKey = "";

            for(int i = 0; i < 4; i++)
                randKey += rand.Next(11000000, 98999999).ToString();

            string randIv = "";

            for (int i = 0; i < 2; i++)
                randIv += rand.Next(11000000, 98999999).ToString();

            log.WriteLine("Generated random encryption stuffs");

            log.Close();

            string out1 = Misc.Enc.Encrypt(randIv);
            string out2 = Misc.Enc.Encrypt(randKey);
            string out3 = Misc.Enc.Encrypt(txtUsername.Text, randIv, randKey);
            string out4 = Misc.Enc.Encrypt(txtPassword.Text, randIv, randKey);
            string out5 = Misc.Enc.Encrypt(hwid, randIv, randKey);
            string lengths = out1.Length.ToString() + out2.Length.ToString() + out3.Length.ToString() + out4.Length.ToString() + out5.Length.ToString();

            Output output = new Output();

            Output.strOutput = lengths + out1 + out2 + out3 + out4 + out5;

            if (!Directory.Exists("C:\\Program Files\\Project Serenity"))
                Directory.CreateDirectory("C:\\Program Files\\Project Serenity");

            StreamWriter swTemp = new StreamWriter(new NetworkCredential("", path2).Password);
            
            for (int i = 0; i < 10; i++)
                swTemp.WriteLine(rand.Next(0, 2000000000).ToString() + rand.Next(0, 2000000000).ToString());

            swTemp.Close();

            path2.Clear();

            if (File.Exists(new NetworkCredential("", path1).Password))
            {
                File.SetAttributes(new NetworkCredential("", path1).Password, FileAttributes.Normal);
                File.Delete(new NetworkCredential("", path1).Password);
            }

            StreamWriter swHwid = new StreamWriter(new NetworkCredential("", path1).Password);

            swHwid.WriteLine(out1);
            swHwid.WriteLine(out2);
            swHwid.WriteLine(out3);
            swHwid.WriteLine(out4);
            swHwid.WriteLine(out5);

            swHwid.Close();

            File.SetAttributes(new NetworkCredential("", path1).Password, FileAttributes.Hidden);


            Properties.Settings.Default.FirstLaunch = false;
            Properties.Settings.Default.Save();

            output.Show();

            this.Hide();
        }

        bool changedUsername = false;
        bool changedPassword = false;

        private void txtUsername_TextChanged(object sender, EventArgs e)
        {
            if (!changedUsername && this.Text != "Username")
            {
                changedUsername = true;
                txtUsername.Text = "";
                txtUsername.ForeColor = Color.Black;
            }
        }

        private void txtPassword_TextChanged(object sender, EventArgs e)
        {
            if (!changedPassword && this.Text != "Password")
            {
                changedPassword = true;
                txtPassword.Text = "";
                txtPassword.UseSystemPasswordChar = true;
                txtPassword.ForeColor = Color.Black;
            }
        }
    }
}
