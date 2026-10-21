# Future extensions

These are planned capabilities, not active UI features.

1. **Baseline learning:** use exported hardware sessions to define an appliance-specific normal operating range; retain appliance identity, operating state, sample coverage, and data quality. Keep simulation data out of learned baselines.
2. **Health scores:** introduce a documented score only after validating its inputs and reference ranges. Store algorithm version, baseline version, timestamp, coverage, and explanations alongside results.
3. **Failure prediction:** collect labelled examples of normal operation and failures before training/evaluating a model. Display uncertainty and avoid presenting an unvalidated threshold as a failure prediction.
4. **Wi-Fi and continuous collection:** add firmware networking and a secured ingestion API/database, then let the static website read that backend. Keep device credentials and server secrets out of frontend code and public repositories. Include reconnect handling, buffering, timestamp synchronization, and data retention.

The current `analysis` array in session JSON is an extension point. Preserve raw readings; add versioned derived results rather than overwriting measurements. Any new transport should normalize data to the same units and distinguish missing readings from valid zeros.

Website entry points: `clean()` validates measurements, `ingest()` records them, `stats()` computes session summaries, and `review()` renders saved results. No algorithm implementation or backend is bundled in this export.
