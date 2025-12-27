Deployment guide — free providers

Overview
- The repository contains a small Node server (`server.js`) that serves the static site and provides realtime updates via Server-Sent Events (`/events`) and a REST API (`/api/status`).
- GitHub Pages only serves static files and cannot host `server.js`.
- Use a free-tier host for the Node server. Below are recommended free providers and quick steps.

Prepare the repo
1. Ensure `package.json` exists (already added) and `start` script runs `node server.js`.
2. Set a secure `ADMIN_TOKEN` environment variable in your host for production. Local default token: `localdev`.

Option A — Render (recommended free web service)
1. Create a free account on https://render.com.
2. New -> Web Service -> Connect your GitHub repository.
3. Branch: choose the branch that contains `server.js` (e.g., `gh-pages` or `main`).
4. Build command: `npm install`
5. Start command: `npm start`
6. Environment:
   - `PORT` (optional, default 8000)
   - `ADMIN_TOKEN` (set to a secure value)
7. Deploy. Render will build and provide a public URL. Use that URL as the backend for the site (if the static site is hosted elsewhere).

Option B — Railway
1. Create a free account at https://railway.app and connect your GitHub repo.
2. Create a new project, link the repo branch, and set `Start Command` to `npm start`.
3. Add `ADMIN_TOKEN` in project environment variables.
4. Deploy and use the provided URL.

Option C — Fly.io (free generous allowances)
1. Install Fly CLI and sign up: https://fly.io/docs/
2. From repo root, run `fly launch` and follow prompts (choose organization and app name).
3. Set `PORT` and `ADMIN_TOKEN` via `fly secrets set ADMIN_TOKEN=yourtoken`.
4. `fly deploy` will build using the `Dockerfile` in repo.

Option D — Vercel (static + serverless caveats)
- Vercel is optimized for static sites and serverless functions. Serverless functions are not suitable for long-lived SSE connections.
- If you want a purely static site on Vercel and realtime via a managed service (Pusher/Ably/Firebase), update `website/status.js` to use that service.

Notes & post-deploy steps
- If you deploy the Node server and also host static site on GitHub Pages, update `website/status.js` (or `index.html`) so that the SSE/EventSource uses the deployed server host: `new EventSource('https://your-app.example/events')`.
- To secure the admin endpoint in production, set `ADMIN_TOKEN` to a strong secret in the host environment instead of using `localdev`.

Local testing
- Run locally with:
  ```powershell
  npm install
  npm start
  ```
- Open `http://localhost:8000` and `http://localhost:8000/admin.html`.

If you'd like, I can:
- Create a small `fly.toml` for Fly.io deployment.
- Prepare a Render service YAML for automated deployment.
- Configure a GitHub Actions workflow to auto-deploy to Railway/Render (requires API token).

Tell me which provider you prefer and I will prepare the provider-specific artifacts and workflow (if any).