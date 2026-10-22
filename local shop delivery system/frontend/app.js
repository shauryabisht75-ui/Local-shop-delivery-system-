// LocalKart frontend - plain JavaScript, talks to the C++ backend (server.cpp)

// ---------- helpers ----------
async function api(url, data) {
  let options = {};
  if (data) {
    options = {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: new URLSearchParams(data).toString()
    };
  }
  const res = await fetch(url, options);
  return res.json();
}

function showMessage(text, good) {
  const box = document.getElementById("message");
  box.textContent = text;
  box.className = "message " + (good ? "good" : "bad");
  setTimeout(() => box.classList.add("hidden"), 5000);
}

// show the message returned by the backend
function report(result) {
  showMessage(result.message, result.ok);
  return result.ok;
}

function safe(text) {   // stop html injection
  return String(text).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

function statusBadge(o) {
  return '<span class="status s' + o.status + '">' + o.statusText + "</span>";
}

function customerName() {
  return document.getElementById("custName").value.trim();
}

function saveName() {
  localStorage.setItem("localkartName", customerName());
}

// ---------- tabs ----------
function showTab(name) {
  ["customer", "shop", "agent"].forEach(t => {
    document.getElementById(t).classList.toggle("hidden", t !== name);
    document.getElementById("tab-" + t).classList.toggle("active", t === name);
  });
  if (name === "customer") { loadShops(); loadCart(); loadMyOrders(); }
  if (name === "shop") loadShopPage();
  if (name === "agent") loadAgentPage();
}

// ---------- CUSTOMER ----------
async function loadShops() {
  const shops = await api("/api/shops");
  let html = "";
  shops.forEach(s => {
    html += '<div class="shopname">' + safe(s.name) + " (" + safe(s.area) + ") - Shop ID " + s.id + "</div>";
    html += "<table><tr><th>ID</th><th>Product</th><th>Price</th><th>Stock</th><th></th></tr>";
    s.products.forEach(p => {
      html += "<tr><td>" + p.id + "</td><td>" + safe(p.name) + "</td><td>Rs " + p.price + "</td>";
      if (p.stock > 0) {
        html += "<td>" + p.stock + '</td><td><input type="number" id="q' + s.id + "_" + p.id +
                '" value="1" min="1" style="width:60px">' +
                '<button class="small" onclick="addToCart(' + s.id + "," + p.id + ')">Add</button></td>';
      } else {
        html += '<td class="out">OUT OF STOCK</td><td></td>';
      }
      html += "</tr>";
    });
    html += "</table>";
  });
  document.getElementById("shopList").innerHTML = html;
}

async function addToCart(shopId, productId) {
  if (!customerName()) { showMessage("Please enter your name first.", false); return; }
  const qty = document.getElementById("q" + shopId + "_" + productId).value;
  report(await api("/api/cart/add", { customer: customerName(), shopId, productId, qty }));
  loadCart();
}

async function comparePrices() {
  const key = document.getElementById("searchBox").value.trim();
  if (!key) { showMessage("Type a product name to compare.", false); return; }
  const offers = await api("/api/compare?q=" + encodeURIComponent(key));
  const box = document.getElementById("compareResult");
  if (offers.length === 0) { box.innerHTML = "<p>No shop sells \"" + safe(key) + "\".</p>"; return; }
  let html = "<table><tr><th>No.</th><th>Product</th><th>Shop</th><th>Area</th><th>Price</th><th>Stock</th><th></th></tr>";
  offers.forEach((o, i) => {
    html += '<tr class="' + (i === 0 ? "cheapest" : "") + '"><td>' + (i + 1) + "</td><td>" + safe(o.product.name) +
            "</td><td>" + safe(o.shop) + "</td><td>" + safe(o.area) + "</td><td>Rs " + o.product.price +
            (i === 0 ? " (cheapest)" : "") + "</td><td>" + o.product.stock + "</td><td>";
    if (o.product.stock > 0)
      html += '<button class="small" onclick="quickAdd(' + o.shopId + "," + o.product.id + ')">Add 1</button>';
    html += "</td></tr>";
  });
  box.innerHTML = html + "</table>";
}

async function quickAdd(shopId, productId) {
  if (!customerName()) { showMessage("Please enter your name first.", false); return; }
  report(await api("/api/cart/add", { customer: customerName(), shopId, productId, qty: 1 }));
  loadCart();
}

async function loadCart() {
  const box = document.getElementById("cartBox");
  const form = document.getElementById("checkoutForm");
  const cart = await api("/api/cart?customer=" + encodeURIComponent(customerName()));
  if (cart.items.length === 0) {
    box.innerHTML = "<p>Your cart is empty.</p>";
    form.classList.add("hidden");
    return;
  }
  let html = "<table><tr><th>No.</th><th>Product</th><th>Shop</th><th>Price</th><th>Qty</th><th>Subtotal</th><th></th></tr>";
  cart.items.forEach(c => {
    html += "<tr><td>" + c.no + "</td><td>" + safe(c.product) + "</td><td>" + safe(c.shop) + "</td><td>" + c.price +
            "</td><td>" + c.qty + "</td><td>" + c.subtotal +
            '</td><td><button class="small red" onclick="removeFromCart(' + c.no + ')">Remove</button></td></tr>';
  });
  html += "</table><p><b>Cart total: Rs " + cart.total + "</b></p>";
  box.innerHTML = html;
  form.classList.remove("hidden");
}

async function removeFromCart(position) {
  report(await api("/api/cart/remove", { customer: customerName(), position }));
  loadCart();
}

async function checkout() {
  const result = await api("/api/checkout", {
    customer: customerName(),
    area: document.getElementById("areaSelect").value,
    address: document.getElementById("address").value
  });
  if (report(result)) document.getElementById("address").value = "";
  loadCart(); loadShops(); loadMyOrders();
}

async function loadMyOrders() {
  const box = document.getElementById("myOrders");
  if (!customerName()) { box.innerHTML = "<p>Enter your name to see your orders.</p>"; return; }
  const list = await api("/api/orders?customer=" + encodeURIComponent(customerName()));
  if (list.length === 0) { box.innerHTML = "<p>You have no orders yet.</p>"; return; }
  let html = "<table><tr><th>Order</th><th>Item</th><th>Shop</th><th>Total</th><th>Deliver to</th><th>Agent</th><th>Status</th></tr>";
  list.forEach(o => {
    html += "<tr><td>#" + o.id + "</td><td>" + o.qty + " x " + safe(o.product) + "</td><td>" + safe(o.shop) +
            "</td><td>Rs " + o.total + "</td><td>" + safe(o.address) + " (" + safe(o.area) + ")</td><td>" +
            (o.agentId ? o.agentId : "-") + "</td><td>" + statusBadge(o) + "</td></tr>";
  });
  box.innerHTML = html + "</table>";
}

// ---------- SHOPKEEPER ----------
function shopId() { return document.getElementById("shopSelect").value; }

async function loadShopPage() {
  const shops = await api("/api/shops");
  const sel = document.getElementById("shopSelect");
  const keep = sel.value;
  sel.innerHTML = shops.map(s => '<option value="' + s.id + '">' + safe(s.name) + " (" + safe(s.area) + ")</option>").join("");
  if (keep) sel.value = keep;

  const shop = shops.find(s => String(s.id) === sel.value);
  let html = "<table><tr><th>ID</th><th>Name</th><th>Price</th><th>Qty</th><th></th></tr>";
  if (shop) shop.products.forEach(p => {
    html += "<tr><td>" + p.id + "</td><td>" + safe(p.name) + "</td><td>" + p.price + "</td><td>" + p.stock +
            '</td><td><button class="small" onclick="fillForm(' + p.id + ",'" + safe(p.name).replace(/'/g, "") + "'," + p.price + "," + p.stock + ')">Edit</button>' +
            '<button class="small red" onclick="deleteProduct(' + p.id + ')">Delete</button></td></tr>';
  });
  document.getElementById("stockTable").innerHTML = shop && shop.products.length ? html + "</table>" : "<p>No products in stock.</p>";
  loadShopOrders();
}

function fillForm(id, name, price, stock) {
  document.getElementById("pId").value = id;
  document.getElementById("pName").value = name;
  document.getElementById("pPrice").value = price;
  document.getElementById("pStock").value = stock;
}

function productData() {
  return {
    shopId: shopId(),
    id: document.getElementById("pId").value,
    name: document.getElementById("pName").value.trim(),
    price: document.getElementById("pPrice").value,
    stock: document.getElementById("pStock").value
  };
}

async function addProduct() { report(await api("/api/shop/product/add", productData())); loadShopPage(); }
async function updateProduct() { report(await api("/api/shop/product/update", productData())); loadShopPage(); }
async function deleteProduct(id) {
  if (!confirm("Delete product " + id + "?")) return;
  report(await api("/api/shop/product/delete", { shopId: shopId(), id }));
  loadShopPage();
}

async function acceptOrder() {
  report(await api("/api/shop/accept", { shopId: shopId() }));
  loadShopOrders();
}

async function loadShopOrders() {
  const list = await api("/api/shop/orders?shopId=" + shopId());
  const box = document.getElementById("shopOrders");
  if (list.length === 0) { box.innerHTML = "<p>No orders for your shop yet.</p>"; return; }
  let html = "<table><tr><th>Order</th><th>Customer</th><th>Item</th><th>Total</th><th>Status</th></tr>";
  list.forEach(o => {
    html += "<tr><td>#" + o.id + "</td><td>" + safe(o.customer) + "</td><td>" + o.qty + " x " + safe(o.product) +
            "</td><td>Rs " + o.total + "</td><td>" + statusBadge(o) + "</td></tr>";
  });
  box.innerHTML = html + "</table>";
}

// ---------- DELIVERY AGENT ----------
async function addAgent() {
  const result = await api("/api/agents/add", {
    name: document.getElementById("agentName").value.trim(),
    area: document.getElementById("agentArea").value
  });
  if (report(result)) document.getElementById("agentName").value = "";
  loadAgentPage();
}

async function loadAgentPage() {
  const agents = await api("/api/agents");
  let html = "<table><tr><th>ID</th><th>Name</th><th>Location</th><th>Status</th></tr>";
  agents.forEach(a => {
    html += "<tr><td>" + a.id + "</td><td>" + safe(a.name) + "</td><td>" + safe(a.area) + "</td><td>" +
            (a.available ? "Available" : "Busy") + "</td></tr>";
  });
  document.getElementById("agentTable").innerHTML = html + "</table>";

  const list = await api("/api/delivery/orders");
  const box = document.getElementById("allOrders");
  if (list.length === 0) { box.innerHTML = "<p>No orders available.</p>"; return; }
  html = "<table><tr><th>Order</th><th>Customer</th><th>Item</th><th>Area</th><th>Agent</th><th>Status</th><th>Action</th></tr>";
  list.forEach(o => {
    let action = "";
    if (o.status === 1 && o.agentId === 0)
      action = '<button class="small" onclick="assign(' + o.id + ')">Assign nearest agent</button>';
    else if (o.status === 1)
      action = '<button class="small" onclick="complete(' + o.id + ')">Mark delivered</button>';
    html += "<tr><td>#" + o.id + "</td><td>" + safe(o.customer) + "</td><td>" + o.qty + " x " + safe(o.product) +
            " (" + safe(o.shop) + ")</td><td>" + safe(o.area) + "</td><td>" + (o.agentId ? o.agentId : "not assigned") +
            "</td><td>" + statusBadge(o) + "</td><td>" + action + "</td></tr>";
  });
  box.innerHTML = html + "</table>";
}

async function assign(orderId) { report(await api("/api/delivery/assign", { orderId })); loadAgentPage(); }
async function complete(orderId) { report(await api("/api/delivery/complete", { orderId })); loadAgentPage(); }

// ---------- start ----------
async function init() {
  const areas = await api("/api/areas");
  const options = areas.map(a => '<option value="' + a.id + '">' + safe(a.name) + "</option>").join("");
  document.getElementById("areaSelect").innerHTML = options;
  document.getElementById("agentArea").innerHTML = options;
  document.getElementById("custName").value = localStorage.getItem("localkartName") || "";
  showTab("customer");
}
init();
