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
using System.Text.RegularExpressions;

namespace Keygen
{
    public partial class Keygen : Form
    {
        public Keygen()
        {
            InitializeComponent();
        }

        private void butRegister_Click(object sender, EventArgs e)
        {
            string strStart = txtKey.Text;

            Regex.Replace(strStart, @"\s+", "");

            int len1 = Convert.ToInt32(strStart.Substring(0, 2));
            int len2 = Convert.ToInt32(strStart.Substring(2, 2));
            int len3 = Convert.ToInt32(strStart.Substring(4, 2));
            int len4 = Convert.ToInt32(strStart.Substring(6, 2));
            int len5 = Convert.ToInt32(strStart.Substring(8, 2));

            int cur = 10;

            string str1 = strStart.Substring(cur, len1);
            cur += len1;
            string str2 = strStart.Substring(cur, len2);
            cur += len2;
            string str3 = strStart.Substring(cur, len3);
            cur += len3;
            string str4 = strStart.Substring(cur, len4);
            cur += len4;
            string str5 = strStart.Substring(cur, len5);
            cur += len5;

            string iv = Enc.Decrypt(str1);
            string key = Enc.Decrypt(str2);

            string username = Enc.Decrypt(str3, iv, key);
            string password = Enc.Decrypt(str4, iv, key);
            string hwid = Enc.Decrypt(str5, iv, key);

            SecureString ftpPass = new NetworkCredential("", "Password").SecurePassword;
            SecureString ftpUser = new NetworkCredential("", "user").SecurePassword;
            SecureString ftpServer = new NetworkCredential("", "url").SecurePassword;

            Ftp ftp = new Ftp(new NetworkCredential("", ftpServer).Password, new NetworkCredential("", ftpUser).Password, new NetworkCredential("", ftpPass).Password);

            StreamWriter sw = new StreamWriter("temp.file");

            sw.WriteLine(str1);
            sw.WriteLine(str2);
            sw.WriteLine(str3);
            sw.WriteLine(str4);
            sw.WriteLine(str5);

            sw.Close();


            string unOnServer = Regex.Replace(str3, "[^0-9a-zA-Z]+", "");

            ftp.createDirectory("htdocs/" + unOnServer + "/");


            using (WebClient client = new WebClient())
            {
                client.Credentials = new NetworkCredential(new NetworkCredential("", ftpUser).Password, new NetworkCredential("", ftpPass).Password);
                client.UploadFile((new NetworkCredential("", ftpServer).Password + "/htdocs/" + unOnServer + "/details.enc"), "STOR", "temp.file");
            }
        }
    }
}
