namespace SonoSage.ViewModels
{
    public class LeituraVm
    {
        public DateTime? DataLeitura { get; set; }
        public int? Db { get; set; }

        public string DateOnlyString => DataLeitura?.ToString("yyyy-MM-dd") ?? string.Empty;
        public string TimeOnlyString => DataLeitura?.ToString("hh:mm:ss tt") ?? string.Empty;

        public string GoogleDate => (DataLeitura.HasValue)
  ? string.Format("Date({0})", DataLeitura.Value.ToString("yyyy,M,d,H,m,s,f"))
  : string.Empty;
        public string DecibelString => (Db != null) ? Db.Value.ToString("###.0") : "0.0";

    }
}
