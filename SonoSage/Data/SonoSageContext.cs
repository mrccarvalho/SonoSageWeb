using SonoSage.Models;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Query.SqlExpressions;
using System.Diagnostics.Metrics;
using System.Reflection;

namespace SonoSage.Data
{
    public class SonoSageContext : DbContext
    {
        public SonoSageContext(DbContextOptions<SonoSageContext> options) : base(options)
        {
        }


        public DbSet<LeituraSensor> LeituraSensores { get; set; }


        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
           

            modelBuilder.Entity<LeituraSensor>(d =>
            { d.Property(e => e.LeituraId).ValueGeneratedOnAdd(); });

            modelBuilder.Entity<LeituraSensor>().HasKey(x => x.LeituraId);

        }

     
    }
}
