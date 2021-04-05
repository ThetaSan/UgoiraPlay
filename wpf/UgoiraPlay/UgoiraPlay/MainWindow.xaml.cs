using System;
using System.Timers;
using System.Windows;
using System.Windows.Media.Imaging;

namespace UgoiraPlay
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            now_timer.Elapsed += UgoiraUpdate;
            Closing += (s, e) => { now_timer.Stop(); };
        }

        private UgoiraFile now_ugoira;
        private int now_ugoira_frame_index = 0;
        private Timer now_timer = new Timer();
        public void FileDrop(object sender, DragEventArgs ev)
        {
            if (!ev.Data.GetDataPresent(DataFormats.FileDrop)) { return; }
            Activate();
            string file = (ev.Data.GetData(DataFormats.FileDrop) as string[])[0];
            var ugoira = new UgoiraFile(file);
            if (ugoira.IsNull)
            {
                var error_type = ugoira.LastError.GetType();
                if (error_type == typeof(UgoiraFrameFileNotFoundException) ||
                    error_type == typeof(UgoiraMetaInvalidException) ||
                    error_type == typeof(UgoiraMetaNotFoundException))
                {
                    MessageBox.Show(".Ugoira error! :\n" + ugoira.LastError.Message,
                        "Error!",
                        MessageBoxButton.OK,
                        MessageBoxImage.Error);
                    return;
                }
                else // droped img
                {
                    now_timer.Stop();
                    now_ugoira?.Dispose();
                    ugoira.Dispose();
                    try
                    {
                        render_zone.Source = new BitmapImage(new Uri(file));
                    }
                    catch (NotSupportedException)
                    {
                        MessageBox.Show("Unsupported format!",
                            "Error!",
                            MessageBoxButton.OK,
                            MessageBoxImage.Error);
                        return;
                    }
                    render_label.Content = "";
                    GC.Collect();
                }
            }
            else
            { // ugoira
                now_timer.Stop();
                now_ugoira?.Dispose();
                now_ugoira = ugoira;
                GC.Collect();
                now_ugoira_frame_index = 0;
                UgoiraUpdate(null, null); // init;
                now_timer.Start();
                render_label.Content = "";
                return;
            }
        }

        private void UgoiraUpdate(object s, EventArgs e)
        {
            if(now_ugoira.IsNull || !now_timer.Enabled) { return; }
            var frame = now_ugoira.FrameInfos[now_ugoira_frame_index];
            int max_frame = now_ugoira.FrameInfos.Length;
            Dispatcher.Invoke(() =>
            {
                if (now_ugoira.IsNull || !now_timer.Enabled) { return; }
                // reduce memory usage
                render_zone.Source = LoadImage(frame.content_data);
            });
            now_timer.Interval = frame.delay;
            now_ugoira_frame_index++;
            if (now_ugoira_frame_index >= max_frame)
            {
                now_ugoira_frame_index = 0;
            }
        }

        private static BitmapImage LoadImage(byte[] imageData)
        {
            if (imageData == null || imageData.Length == 0) return null;
            var image = new BitmapImage();
            using (var mem = new System.IO.MemoryStream(imageData))
            {
                mem.Position = 0;
                image.BeginInit();
                image.CreateOptions = BitmapCreateOptions.PreservePixelFormat;
                image.CacheOption = BitmapCacheOption.OnLoad;
                image.UriSource = null;
                image.StreamSource = mem;
                image.EndInit();
            }
            image.Freeze();
            return image;
        }
    }
}
