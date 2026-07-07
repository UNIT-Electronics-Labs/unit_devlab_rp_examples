import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"","description":"","frontmatter":{"layout":"home","hero":{"name":"Gowin FPGA Labs","text":"Ejemplos DevLab para FPGA Gowin","tagline":"Documentacion practica para compilar, cargar y estudiar ejemplos en Verilog y VHDL sin amarrar el SDK a una tarjeta especifica.","actions":[{"theme":"brand","text":"Ver ejemplos","link":"/examples/"},{"theme":"alt","text":"Usar DevLab","link":"/guide/devlab"}]},"features":[{"title":"Flujo portable","details":"Usa DevLab como entrada principal para evitar depender de Make en Windows o macOS."},{"title":"Dos HDLs","details":"Cada practica conserva Verilog como flujo por defecto y VHDL como variante explicita."},{"title":"Target configurable","details":"La referencia actual usa un GW1NR-LV9QN88PC6/I5, pero la tarjeta se define por los archivos CST y la seccion flash."}]},"headers":[],"relativePath":"index.md","filePath":"index.md"}');
const _sfc_main = { name: "index.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h2 id="mapa-rapido" tabindex="-1">Mapa Rapido <a class="header-anchor" href="#mapa-rapido" aria-label="Permalink to &quot;Mapa Rapido&quot;">​</a></h2><ul><li><a href="./guide/devlab">Guia de DevLab</a></li><li><a href="./guide/verilog">Introduccion a Verilog</a></li><li><a href="./guide/vhdl">Introduccion a VHDL</a></li><li><a href="./guide/cst">Archivos CST</a></li><li><a href="./examples/">Ejemplos basicos</a></li><li><a href="./examples/digital-labs/">Digital Labs</a></li></ul><h2 id="uso-basico" tabindex="-1">Uso Basico <a class="header-anchor" href="#uso-basico" aria-label="Permalink to &quot;Uso Basico&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">npm</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> install</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">npm</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> run</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> docs:dev</span></span></code></pre></div><p>El sitio se sirve localmente con VitePress y documenta los ejemplos que viven en este repositorio.</p></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("index.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const index = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  index as default
};
