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
using System.Threading;
using System.Text.RegularExpressions;

namespace Interface.Forms
{
    public partial class Loader : Form
    {
        [DllImport("Kernel32.dll", SetLastError = true)]
        extern static bool GetVolumeInformation(string vol, StringBuilder name, int nameSize, out uint serialNum, out uint maxNameLen, out uint flags, StringBuilder fileSysName, int fileSysNameSize);

        private void CheckForReset()
        {

        }

        public Loader()
        {
            InitializeComponent();
        }

        bool changedUsername = false;
        bool changedPassword = false;

        private void txtUsername_TextChanged(object sender, EventArgs e)
        {
            if(!changedUsername && this.Text != "Username")
            {
                changedUsername = true;
                txtUsername.Text = "";
                txtUsername.ForeColor = Color.Black;
            }
        }

        private void txtPassword_TextChanged(object sender, EventArgs e)
        {
            if (!changedPassword && this.Text != "Username")
            {
                changedPassword = true;
                txtPassword.Text = "";
                txtPassword.UseSystemPasswordChar = true;
                txtPassword.ForeColor = Color.Black;
            }
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

        private void butLogin_Click(object sender, EventArgs e)
        {
            SecureString path1 = new NetworkCredential("", "C:\\details.enc").SecurePassword;
            SecureString path2 = new NetworkCredential("", "C:\\Program Files\\Project Serenity\\details.enc").SecurePassword;
            SecureString pathDetails = new NetworkCredential("", "details.enc").SecurePassword;
            
            StreamReader srHwid1;
            try
            {
                srHwid1 = new StreamReader(new NetworkCredential("", path1).Password);
                Thread.Sleep(5);
            }

            catch (Exception)
            {
                MessageBox.Show("Error! You don't appear to be registered. This should never happen");
                return;
            }

            if (!File.Exists(new NetworkCredential("", path2).Password))
                return;

            else
                File.Delete(new NetworkCredential("", path2).Password);

            StreamWriter swTemp = new StreamWriter(new NetworkCredential("", path2).Password);

            Random rand = new Random();

            for(int i = 0; i < 10; i++)
                swTemp.WriteLine(rand.Next(0, 2000000));

            swTemp.Close();

            if (File.Exists("Serenity.log"))
            {
                File.Delete("Serenity.log");
            }

            if (File.Exists(new NetworkCredential("", pathDetails).Password))
            {
                File.Delete(new NetworkCredential("", pathDetails).Password);
            }

            StreamWriter log = new StreamWriter("Serenity.log");

            log.WriteLine("Getting HWID");

            string tempHwid1 = GetHwid1("C:\\").ToString();
            string tempHwid2 = GetHwid2();
            string thisHwid = tempHwid1 + tempHwid2;
            thisHwid = thisHwid.ToLower();

            log.WriteLine("Got HWID");

            SecureString ftpPass = new NetworkCredential("", "Hadsfsdfsdf9").SecurePassword;
            SecureString ftpUser = new NetworkCredential("", "basdasd3").SecurePassword;
            SecureString ftpServer = new NetworkCredential("", "urlllllllllllllllllllllllllllllllllllll fucking kill me").SecurePassword;

            log.WriteLine("Connecting to ftp");

            Misc.Ftp ftp = new Misc.Ftp(new NetworkCredential("", ftpServer).Password, new NetworkCredential("", ftpUser).Password, new NetworkCredential("", ftpPass).Password);

            log.WriteLine("Connected");
            
            log.WriteLine("Searching for user");

            string[] dirs = ftp.directoryListSimple("htdocs/");

            bool foundUser = false;

            string iv = Misc.Enc.Decrypt(srHwid1.ReadLine());
            string key = Misc.Enc.Decrypt(srHwid1.ReadLine());

            string encUsername = srHwid1.ReadLine();

            string username = Misc.Enc.Decrypt(encUsername, iv, key);
            string encPassword = srHwid1.ReadLine();
            string hwid = Misc.Enc.Decrypt(srHwid1.ReadLine(), iv, key);

            srHwid1.Close();

            string unOnServer = Regex.Replace(encUsername, "[^0-9a-zA-Z]+", "");


            if (hwid != thisHwid)
            {
                log.WriteLine("HWID doesn't match");
                MessageBox.Show("Incorrect HWID");
            }

            foreach (string dir in dirs)
            {
                if (dir == unOnServer && username.ToLower() == txtUsername.Text.ToLower())
                {
                    log.WriteLine("Found User " + username);
                    foundUser = true;
                }
            }

            if (!foundUser)
            {
                MessageBox.Show("We didn't recognise that username. Please note it is case sensitive");
                return;
            }
            
            try
            {
                ftp.download("htdocs/" + unOnServer + "/details.enc", "details.enc");
                Thread.Sleep(5);
                File.SetAttributes(new NetworkCredential("", pathDetails).Password, FileAttributes.Hidden);
            }

            catch (Exception)
            {

            }

            if (!File.Exists("details.enc"))
            {
                log.WriteLine("Error With Ftp Server");
                return;
            }

            StreamReader srHwid2 = new StreamReader(new NetworkCredential("", pathDetails).Password);

            srHwid2.ReadLine();
            srHwid2.ReadLine();

            string encUsername2 = srHwid2.ReadLine();
            
            log.WriteLine("Getting remote login info");

            string username2 = Misc.Enc.Decrypt(encUsername, iv, key);
            string encPassword2 = srHwid2.ReadLine();
            string hwid2 = Misc.Enc.Decrypt(srHwid2.ReadLine(), iv, key);

            if (hwid2 != thisHwid || hwid != hwid2)
            {
                log.WriteLine("HWID doesn't match");
                MessageBox.Show("Incorrect HWID");
            }

            if (encPassword != encPassword2)
            {
                log.WriteLine("Password doesn't match");
                return;
            }

            log.WriteLine("Checking Password");

            if(encPassword != Misc.Enc.Encrypt(txtPassword.Text, iv, key))
            {
                log.WriteLine("Error password doesn't match");
                MessageBox.Show("Password incorrect");
            }

            MessageBox.Show("Login Successful!");
        }
    }
}
