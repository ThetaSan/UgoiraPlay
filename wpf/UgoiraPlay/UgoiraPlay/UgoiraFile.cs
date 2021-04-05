using System;
using System.IO;
using System.IO.Compression;
using System.Text;
using Newtonsoft.Json;

namespace UgoiraPlay
{
    public class UgoiraFile : IDisposable
    {
        public UgoiraFile() { }
        public UgoiraFile(string path)
        {
            Open(path);
        }
        ~UgoiraFile()
        {
            Dispose();
        }

        public bool IsNull { get; private set; } = true;
        public Exception LastError { get; private set; } = null;
        public UgoiraMeta MetaInfo { get; private set; } = null;

        public UgoiraMetaFrame[] FrameInfos
        {
            get
            {
                return MetaInfo?.body.frames;
            }
        }

        public bool Open(string path)
        {
            Dispose();
            try
            {
                using (var zip = ZipFile.OpenRead(path))
                {
                    string meta_json;
                    var meta_entry = zip.GetEntry("ugoira_meta");
                    if (meta_entry == null) { throw new UgoiraMetaNotFoundException("ugoira_meta does not exist."); }
                    try
                    {
                        using (var meta_stream = meta_entry.Open())
                        using (var str_r = new StreamReader(meta_stream, Encoding.UTF8))
                            meta_json = str_r.ReadToEnd();
                        MetaInfo = JsonConvert.DeserializeObject<UgoiraMeta>(meta_json);
                        if(MetaInfo.body == null) {
                            throw new Exception(); // jump to catch
                        }  
                    }
                    catch
                    {
                        throw new UgoiraMetaInvalidException("ugoira_meta is invalid.");
                    }
                    for (int i = 0; i < FrameInfos.Length; i++)
                    {
                        var file_entry = zip.GetEntry(FrameInfos[i].file);
                        if (file_entry == null)
                        {
                            throw new UgoiraFrameFileNotFoundException($"frame {i + 1}, {FrameInfos[i].file} does not exist.");
                        }
                        using (var fstream = file_entry.Open())
                        {
                            using (var ms = new MemoryStream())
                            {
                                fstream.CopyTo(ms);
                                MetaInfo.body.frames[i].content_data = ms.ToArray();
                            }
                        }
                    }
                    IsNull = false;
                    return true;
                }
            }
            catch (Exception e)
            {
                Dispose();
                LastError = e;
                return false;
            }
        }

        public void Dispose()
        {
            IsNull = true;
            LastError = null;
            MetaInfo = null;

            GC.Collect();
        }

        #region META_CLASSES
        [Serializable]
        public class UgoiraMeta
        {
            public bool error;
            public string message;
            public UgoiraMetaBody body;
        }
        [Serializable]
        public class UgoiraMetaBody
        {
            public string src;
            public string originalSrc;
            public string mime_type;
            public UgoiraMetaFrame[] frames;
        }
        [Serializable]
        public class UgoiraMetaFrame
        {
            public string file;
            public int delay;
            public byte[] content_data;
        }
        #endregion
    }

    public class UgoiraMetaNotFoundException : Exception
    {
        public UgoiraMetaNotFoundException() : base() { }
        public UgoiraMetaNotFoundException(string message, Exception inner = null) : base(message, inner) { }
    }

    public class UgoiraMetaInvalidException : Exception
    {
        public UgoiraMetaInvalidException() : base() { }
        public UgoiraMetaInvalidException(string message, Exception inner = null) : base(message, inner) { }
    }

    public class UgoiraFrameFileNotFoundException : Exception
    {
        public UgoiraFrameFileNotFoundException() : base() { }
        public UgoiraFrameFileNotFoundException(string message, Exception inner = null) : base(message, inner) { }
    }
}
