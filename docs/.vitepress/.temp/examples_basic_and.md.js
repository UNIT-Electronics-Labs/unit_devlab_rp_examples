import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"AND","description":"","frontmatter":{},"headers":[],"relativePath":"examples/basic/and.md","filePath":"examples/basic/and.md"}');
const _sfc_main = { name: "examples/basic/and.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h1 id="and" tabindex="-1">AND <a class="header-anchor" href="#and" aria-label="Permalink to &quot;AND&quot;">​</a></h1><p>Ejemplo de compuerta AND usando dos entradas con pull-up y un LED activo en bajo.</p><h2 id="que-aprendes" tabindex="-1">Que Aprendes <a class="header-anchor" href="#que-aprendes" aria-label="Permalink to &quot;Que Aprendes&quot;">​</a></h2><ul><li>Invertir entradas activas en bajo.</li><li>Construir logica combinacional.</li><li>Mapear el resultado a un LED activo en bajo.</li></ul><h2 id="archivos" tabindex="-1">Archivos <a class="header-anchor" href="#archivos" aria-label="Permalink to &quot;Archivos&quot;">​</a></h2><ul><li><code>and/devlab.toml</code></li><li><code>and/devlab-vhdl.toml</code></li><li><code>and/src/top.v</code></li><li><code>and/src/top.vhd</code></li><li><code>and/pins.cst</code></li></ul><h2 id="compilar" tabindex="-1">Compilar <a class="header-anchor" href="#compilar" aria-label="Permalink to &quot;Compilar&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> and</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="vhdl" tabindex="-1">VHDL <a class="header-anchor" href="#vhdl" aria-label="Permalink to &quot;VHDL&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> and</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}"> -c</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> devlab-vhdl.toml</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="logica" tabindex="-1">Logica <a class="header-anchor" href="#logica" aria-label="Permalink to &quot;Logica&quot;">​</a></h2><p>Las entradas fisicas se invierten dentro del HDL para que el valor logico sea <code>1</code> cuando el boton esta presionado. El LED se enciende cuando ambas entradas logicas valen <code>1</code>.</p></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("examples/basic/and.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const and = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  and as default
};
