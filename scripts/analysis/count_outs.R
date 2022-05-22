# Select evaluation
evaluation <- 2021

# Read results
if (evaluation == 2010) {
    results <- read.csv(file = "./scripts/analysis/results2010_2.csv", stringsAsFactors = FALSE)
    mem_limit <- 512
    time_limit <- 1800
} else {
    results <- read.csv(file = "./scripts/analysis/results2021_deletes_2.csv", stringsAsFactors = FALSE)
    mem_limit <- 32768
    time_limit <- 3600
}
colnames(results) <- c("instance", "runtime", "totalizer", "mem", "runtime_w", "proofsize", "totalizer_w", "totalizer2_w", "mem_w", "runtime_v", "mem_v", "status")
instances <- results$instance
results <- as.data.frame(lapply(results, as.numeric))
results$instance <- instances

# Safety checks
incorrects <- list()
for (row in 1:nrow(results)) {
    # Incorrect instances
    if (!is.na(results[row, "status"]) & results[row, "status"] == 0 
        & !is.na(results[row, "runtime_v"]) & results[row, "runtime_v"] < 10 * time_limit 
        & !is.na(results[row, "mem_v"]) & results[row, "mem_v"] < 2*mem_limit) {
        incorrects <- append(incorrects, row)
    }
}
print(results[unlist(incorrects), "instance"])
results <- results[-unlist(incorrects),]

# VeriPB OOTs
OoTs <- 0
total <- 0
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "runtime_v"]) & results[row, "runtime_v"] >= 10 * time_limit) {
        OoTs <- OoTs + 1
    }
    if (!is.na(results[row, "runtime_w"]) & results[row, "runtime_w"] < time_limit & !is.na(results[row, "mem_w"]) & results[row, "mem_w"] < mem_limit) {
        print(row)
        total <- total + 1
    }
}
OoTs
total

# VeriPB OOMs
OoMs <- 0
total <- 0
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "mem_v"]) & results[row, "mem_v"] >= 2 * mem_limit) {
        OoMs <- OoMs + 1
    }
    if (!is.na(results[row, "runtime_w"]) & results[row, "runtime_w"] < time_limit & !is.na(results[row, "mem_w"]) & results[row, "mem_w"] < mem_limit) {
        total <- total + 1
    }
}
OoMs
total
