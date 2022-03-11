library(ggplot2)
library(grid)
library(ggthemes)
theme_set(theme_light())

# Select evaluation
evaluation <- 2021

# Read results
if (evaluation == 2010) {
    results <- read.csv(file = "./scripts/analysis/results2010.csv", stringsAsFactors = FALSE)
    mem_limit <- 512
    time_limit <- 1800
} else {
    results <- read.csv(file = "./scripts/analysis/results2021_2.csv", stringsAsFactors = FALSE)
    mem_limit <- 32768
    time_limit <- 3600
}
colnames(results) <- c("instance", "runtime", "totalizer", "mem", "runtime_w", "proofsize", "totalizer_w", "totalizer2_w", "mem_w", "runtime_v", "mem_v", "status")
instances <- results$instance
results <- as.data.frame(lapply(results, as.numeric))
results$instance <- instances

# Safety checks
for (row in 1:nrow(results)) {

    # Incorrect instances
    if (!is.na(results[row, "status"]) & results[row, "status"] == 0 & results[row, "runtime_v"] < time_limit & results[row, "mem_v"] < mem_limit) {
        print(results[row, ])
        write(results[row, "instance"], "incorrects.txt", append = TRUE)
        results <- results[-c(row), ]
    }
}

# QMaxSATpb OOTs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "runtime_w"]) & results[row, "runtime_w"] >= time_limit) {
        results[row, "runtime_w"] <- 4000
    }
}

# QMaxSATpb OOMs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "mem_w"]) & results[row, "mem_w"] >= mem_limit) {
        results[row, "runtime_w"] <- 8500
    }
}

# VeriPB OOTs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "runtime_v"]) & results[row, "runtime_v"] >= 10 * time_limit) {
        results[row, "runtime_v"] <- 46000
    }
}

# VeriPB OOMs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "mem_v"]) & results[row, "mem_v"] >= 2 * mem_limit) {
        results[row, "runtime_v"] <- 99000
    }
}


#--------------------------------
# PLOTS
#--------------------------------

# TYPE1

# drop qmaxsat OoT/OoM
no_NAs <- results[!is.na(results$runtime) & results$runtime < time_limit & !is.na(results$mem) & results$mem < mem_limit, ]

ggplot(no_NAs, aes(x = runtime_w, y = runtime, color = log10((proofsize / 10^3) + 1))) +
    geom_point() +
    scale_x_log10(breaks = c(1, 10, 100, 1000)) +
    scale_y_log10(breaks = c(1, 10, 100, 1000)) +
    scale_color_continuous(breaks = c(2, 3, 4, 5, 6, 7), labels = c("100KB", "1MB", "10MB", "100MB", "1GB", "10GB")) +
    coord_fixed(ratio = 1) +
    geom_vline(xintercept = 4000, linetype = "dashed") +
    geom_vline(xintercept = 8500, linetype = "dashed") +
    geom_abline(slope = 1, intercept = 0, linetype = "dashed") +
    labs(color = "Proofsize") +
    xlab("QMaxSATpb (time in s)") +
    ylab("QMaxSAT (time in s)") +
    coord_cartesian(xlim = c(0.1, 10000), ylim = c(0.1, 10000)) +
    annotate(
        geom = "text",
        label = "OoT",
        x = 4250,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    ) +
    annotate(
        geom = "text",
        label = "OoM",
        x = 9100,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    )
ggsave("./scripts/analysis/without_vs_with.pdf", device = "pdf", width = 14, height = 12, units = "cm")

# TYPE2

# drop qmaxsatpb OoT/OoM
no_NAs2 <- results[!is.na(results$runtime_w) & results$runtime_w < time_limit & !is.na(results$mem_w) & results$mem_w < mem_limit, ]

ggplot(no_NAs2, aes(x = runtime_v, y = runtime_w, color = log10((proofsize / 10^3) + 1))) +
    geom_point() +
    scale_x_log10(breaks = c(1, 10, 100, 1000, 10000)) +
    scale_y_log10(breaks = c(1, 10, 100, 1000, 10000)) +
    scale_color_continuous(breaks = c(2, 3, 4, 5, 6, 7), labels = c("100KB", "1MB", "10MB", "100MB", "1GB", "10GB")) +
    coord_fixed(ratio = 1) +
    geom_vline(xintercept = 46000, linetype = "dashed") +
    geom_vline(xintercept = 99000, linetype = "dashed") +
    geom_abline(slope = 1, intercept = 0, linetype = "dashed") +
    xlab("VeriPB (time in s)") +
    ylab("QMaxSATpb (time in s)") +
    labs(color = "Proofsize") +
    coord_cartesian(xlim = c(0.1, 100000), ylim = c(0.1, 10000)) +
    annotate(
        geom = "text",
        label = "OoT",
        x = 49000,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    ) +
    annotate(
        geom = "text",
        label = "OoM",
        x = 105000,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    )
ggsave("./scripts/analysis/solving_vs_verification.pdf", device = "pdf", width = 18, height = 12, units = "cm")
