using System.ComponentModel.DataAnnotations;

namespace SonoSage.Models
{
    public class LeituraSensor
    {
        [Required]
        public int LeituraId { get; set; }

        [Required]
        public int Leitura { get; set; }

        [Required]
        public System.DateTime DataLeitura { get; set; }

    }
}
