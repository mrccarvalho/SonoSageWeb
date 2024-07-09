using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SonoSage.Migrations
{
    public partial class Inicial : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "LeituraSensores",
                columns: table => new
                {
                    LeituraId = table.Column<int>(type: "int", nullable: false)
                        .Annotation("SqlServer:Identity", "1, 1"),
                    Leitura = table.Column<int>(type: "int", nullable: false),
                    DataLeitura = table.Column<DateTime>(type: "datetime2", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_LeituraSensores", x => x.LeituraId);
                });
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "LeituraSensores");
        }
    }
}
