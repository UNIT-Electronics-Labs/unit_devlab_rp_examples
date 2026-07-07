import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"Ejemplos","description":"","frontmatter":{},"headers":[],"relativePath":"examples/index.md","filePath":"examples/index.md"}');
const _sfc_main = { name: "examples/index.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h1 id="ejemplos" tabindex="-1">Ejemplos <a class="header-anchor" href="#ejemplos" aria-label="Permalink to &quot;Ejemplos&quot;">​</a></h1><p>Este repositorio agrupa ejemplos basicos y practicas de sistemas digitales para FPGA Gowin. La referencia actual usa el dispositivo <code>GW1NR-LV9QN88PC6/I5</code>, pero la tarjeta target se puede cambiar ajustando <code>pins.cst</code> y la seccion <code>[flash]</code> de DevLab.</p><h2 id="basicos" tabindex="-1">Basicos <a class="header-anchor" href="#basicos" aria-label="Permalink to &quot;Basicos&quot;">​</a></h2><ul><li><a href="./basic/blink">Blink</a>: contador con reloj y LED.</li><li><a href="./basic/and">AND</a>: compuerta AND con dos entradas.</li><li><a href="./basic/or">OR</a>: compuerta OR con dos entradas.</li><li><a href="./basic/not">NOT</a>: inversor con una entrada.</li></ul><h2 id="digital-labs" tabindex="-1">Digital Labs <a class="header-anchor" href="#digital-labs" aria-label="Permalink to &quot;Digital Labs&quot;">​</a></h2><ul><li><a href="./digital-labs/">Indice de practicas</a></li></ul><h2 id="flujo-recomendado" tabindex="-1">Flujo Recomendado <a class="header-anchor" href="#flujo-recomendado" aria-label="Permalink to &quot;Flujo Recomendado&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> and</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><p>Para VHDL:</p><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}"> -c</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> devlab-vhdl.toml</span></span></code></pre></div></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("examples/index.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const index = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  index as default
};
