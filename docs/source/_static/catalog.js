// Turns the dataset table on built_in_datasets into a searchable, filterable,
// sortable catalog and links every row to its detail section. The markdown
// stays the single source of truth; this only enhances what is rendered.
document.addEventListener("DOMContentLoaded", () => {
  const table = [...document.querySelectorAll("table.docutils")].find(
    (t) => t.querySelector("th")?.textContent.trim() === "Key",
  );
  if (!table) return;

  const FLOWS = [
    ["Xilinx", /xilinx|vitis/i],
    ["Catapult", /catapult/i],
    ["Stratus", /stratus/i],
    ["Google XLS", /xls/i],
    ["Altera", /altera/i],
  ];
  const rows = [...table.tBodies[0].rows];
  const headers = [...table.tHead.rows[0].cells].map((c) => c.textContent.trim());
  const col = (name) => headers.indexOf(name);

  // Detail sections, keyed by dataset key found in the heading's <code>.
  const sections = {};
  document.querySelectorAll("section > h3").forEach((h) => {
    const key = h.querySelector("code")?.textContent.trim();
    if (key) sections[key] = h.parentElement;
  });

  const items = rows.map((row) => {
    const keyCell = row.cells[col("Key")];
    const key = keyCell.textContent.trim();
    const flow = row.cells[col("Flow Coverage")].textContent;
    const optdsl = !/not provided/i.test(row.cells[col("OptDSL Templates")].textContent);
    const designs = parseInt(row.cells[col("Designs")].textContent, 10);
    const section = sections[key];
    if (section) {
      const a = document.createElement("a");
      a.href = "#" + section.id;
      a.className = "catalog-key";
      a.append(...keyCell.childNodes);
      keyCell.replaceChildren(a);
    }
    return {
      row, key, section, optdsl, designs: isNaN(designs) ? 0 : designs,
      flows: FLOWS.filter(([, re]) => re.test(flow)).map(([n]) => n),
      text: row.textContent.toLowerCase(),
    };
  });

  // Each dataset's detail section becomes a card with summary badges.
  items.forEach((i) => {
    if (!i.section) return;
    i.section.classList.add("dataset-card");
    i.section.parentElement.classList.add("dataset-cards");
    const h = i.section.querySelector(":scope > h3");
    const badges = document.createElement("div");
    badges.className = "dataset-badges";
    const add = (txt, cls) => {
      const b = document.createElement("span");
      b.className = "dataset-badge " + (cls || "");
      b.textContent = txt;
      badges.append(b);
    };
    add(i.designs ? `${i.designs} designs` : "varies", "designs");
    i.flows.forEach((f) => add(f));
    if (i.optdsl) add("OptDSL", "optdsl");
    h.after(badges);
  });

  const state = { q: "", flow: null, optdsl: false };

  // Toolbar
  const bar = document.createElement("div");
  bar.className = "catalog-toolbar";
  bar.innerHTML = `
    <div class="catalog-stats"></div>
    <input class="catalog-search" type="search" autocomplete="off"
           placeholder="Filter the table…  (press / to focus)">
    <div class="catalog-chips"></div>
    <div class="catalog-count" aria-live="polite"></div>`;
  table.before(bar);
  const input = bar.querySelector("input");
  const chips = bar.querySelector(".catalog-chips");
  const count = bar.querySelector(".catalog-count");

  const total = items.reduce((s, i) => s + i.designs, 0);
  bar.querySelector(".catalog-stats").innerHTML = [
    [items.length, "datasets"],
    [total + "+", "designs"],
    [new Set(items.flatMap((i) => i.flows)).size, "HLS toolchains"],
  ].map(([n, l]) => `<div class="catalog-stat"><b>${n}</b><span>${l}</span></div>`).join("");

  const chip = (label, onclick) => {
    const b = document.createElement("button");
    b.type = "button";
    b.className = "catalog-chip";
    b.textContent = label;
    b.onclick = () => { onclick(b); apply(); };
    chips.append(b);
    return b;
  };
  const flowChips = {};
  FLOWS.forEach(([name]) => {
    if (!items.some((i) => i.flows.includes(name))) return;
    flowChips[name] = chip(name, () => {
      state.flow = state.flow === name ? null : name;
    });
  });
  const optChip = chip("Has OptDSL templates", () => { state.optdsl = !state.optdsl; });

  function apply() {
    const terms = state.q.toLowerCase().split(/\s+/).filter(Boolean);
    let shown = 0;
    items.forEach((i) => {
      const ok =
        terms.every((t) => i.text.includes(t)) &&
        (!state.flow || i.flows.includes(state.flow)) &&
        (!state.optdsl || i.optdsl);
      i.row.hidden = !ok;
      shown += ok;
    });
    Object.entries(flowChips).forEach(([n, b]) => b.classList.toggle("active", state.flow === n));
    optChip.classList.toggle("active", state.optdsl);
    count.textContent = shown === items.length
      ? `Showing all ${shown} datasets`
      : `${shown} of ${items.length} datasets match`;
  }
  input.addEventListener("input", () => { state.q = input.value; apply(); });
  document.addEventListener("keydown", (e) => {
    if (e.key === "/" && !/input|textarea/i.test(document.activeElement.tagName)) {
      e.preventDefault();
      input.focus();
    }
  });

  // Sortable columns
  let sortCol = -1, asc = true;
  [...table.tHead.rows[0].cells].forEach((th, idx) => {
    th.classList.add("sortable");
    th.onclick = () => {
      asc = sortCol === idx ? !asc : true;
      sortCol = idx;
      const val = (r) => r.cells[idx].textContent.trim().toLowerCase();
      const num = headers[idx] === "Designs";
      const sorted = [...items].sort((a, b) => {
        const x = num ? a.designs : val(a.row), y = num ? b.designs : val(b.row);
        return (x > y ? 1 : x < y ? -1 : 0) * (asc ? 1 : -1);
      });
      sorted.forEach((i) => table.tBodies[0].append(i.row));
      table.tHead.querySelectorAll("th").forEach((h) => h.removeAttribute("data-sort"));
      th.dataset.sort = asc ? "asc" : "desc";
    };
  });

  // Deep link: ?q=polybench
  const q = new URLSearchParams(location.search).get("q");
  if (q) { input.value = q; state.q = q; }
  apply();
});
