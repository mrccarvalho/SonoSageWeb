using SonoSage.Data;
using SonoSage.Models;
using SonoSage.ViewModels;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

using System.Diagnostics;
using System.Diagnostics.Metrics;

namespace SonoSage.Controllers
{
    public class HomeController : Controller
    {
        private readonly ILogger<HomeController> _logger;
        private readonly SonoSageContext _context;

        public HomeController(ILogger<HomeController> logger, SonoSageContext context)
        {
            _logger = logger;
            _context = context;
        }
        public IActionResult Index()
        {
            var vm = new RelatorioVm { };

            vm.LastSet = MaisRecentes();

            return View(vm);
        }

        public IActionResult Grafico()
        {
            var vm = new RelatorioVm { };

            vm.LastSet = MaisRecentes();

            return View(vm);
        }

        /// <summary>
        /// devolve leituras mais recentes
        /// </summary>
        /// <returns></returns>
        public LeituraVm MaisRecentes()
        {
            var ultimaVm = new LeituraVm();

            var ultimaLeitura = _context.LeituraSensores.
                OrderByDescending(m => m.DataLeitura).Take(1).ToList();

            if (ultimaLeitura.Any())
            {
                var dec = ultimaLeitura.FirstOrDefault();


                if (dec != null)
                {
                    ultimaVm.DataLeitura = dec.DataLeitura;
                    ultimaVm.Db = dec.Leitura;
                }

         
            }

            return ultimaVm;
        }

       

        /// <summary>
        /// devolve leituras
        /// </summary>
        /// <returns></returns>
        public IActionResult ListaTodasLeituras(int localizacaoId)
        {
            List<LeituraSensor> ultimasLeituras = new List<LeituraSensor>();

            ultimasLeituras = _context.LeituraSensores.
                OrderByDescending(m => m.DataLeitura).ToList();

            return View(ultimasLeituras);
        }


        /// <summary>
        /// Método que recebe os valores do sensor e grava na base de dados
        /// </summary>
        /// <param name="dB"></param>
        /// <returns></returns>
        public ActionResult InserirLeituras(int? dB)
        {
            var resultado = "Método ok";
            var dataLeitura = DateTime.Now;
            try
            {
                    if (dB.HasValue)
                    {
                        // adiciona leitura dos decibeis
                        _context.LeituraSensores.Add(new LeituraSensor
                        {
                            Leitura = dB.Value,
                            DataLeitura = dataLeitura
                        });
                    }
                    _context.SaveChanges();   
            }
            catch (Exception ex)
            {
                resultado = "Erro: " + ex.Message;
            }
            return Content(resultado);
        }

        /// <summary>
        /// Devolve dados por dia/24 horas
        /// </summary>
        /// <returns></returns>
        public IActionResult LeiturasUltimas24Horas()
        {
            // cria uma datatable vazia
            var gdataTable = new GoogleVizDataTable();


                // obter a leitura mais recente
                var maisRecente = _context.LeituraSensores
                    .Select(m => m).OrderByDescending(m => m.DataLeitura).Take(1).FirstOrDefault();

                // se tiver uma leitura mais recente
                if (maisRecente != null)
                {
                    // estabelece um intervalo de tempo
                    var finish = maisRecente.DataLeitura;
                    var start = finish.AddDays(-1);

                    // obtem um conjunto de leitura dentro do intervalo definido
                    var recentSet = MedicaoSetRange(start, finish);

                return Json(recentSet);
            }

            return NotFound();

          
        }

        /// <summary>
        /// lista de leituras dentro do intervalo.
        /// </summary>
        /// <param name="start">Início date/time </param>
        /// <param name="finish">Fim date/time </param>
        /// <returns></returns>
        public List<LeituraVm> MedicaoSetRange(DateTime start, DateTime finish)
        {
            // constrói o conjunto de medições
            var measureSet =
                (from m in _context.LeituraSensores.Select(m => m).AsEnumerable()
                 where m.DataLeitura >= start && m.DataLeitura <= finish
                 orderby m.DataLeitura
                 group m by new { MeasuredDate = DateTime.Parse(m.DataLeitura.ToString("yyyy-MM-dd HH:mm:ss"))}
                    into g
                 select new LeituraVm
                 {
                     DataLeitura = g.Key.MeasuredDate,
                     
                     Db = g.Select(r => r.Leitura).FirstOrDefault(),

                 }).ToList();

            return measureSet;
        }
        public IActionResult Todas()
        {
            return View();
        }

        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
        public IActionResult Error()
        {
            return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
        }
    }
}
